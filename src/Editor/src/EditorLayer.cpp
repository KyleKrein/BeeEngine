//
// Created by alexl on 26.05.2023.
//
#include "EditorLayer.h"
#include "AssetScanner.h"
#include "ConfigFile.h"
#include "Core/Application.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/AssetManagement/AssetRegistrySerializer.h"
#include "Core/AssetManagement/EngineAssetRegistry.h"
#include "Core/Events/EventImplementations.h"
#include "Core/Logging/Log.h"
#include "Core/ResourceManager.h"
#include "Core/TypeDefines.h"
#include "Debug/Instrumentor.h"
#include "FileSystem/File.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Gui/MessageBox.h"
#include "JobSystem/JobScheduler.h"
#include "Locale/Locale.h"
#include "Locale/LocalizationGenerator.h"
#include "Panels/ProjectSettings.h"
#include "Platform/ImGui/ImGuiController.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scripting/MAssembly.h"
#include "Scripting/MClass.h"
#include "Scripting/NativeToManaged.h"
#include "Scripting/ScriptGlue.h"
#include "Scripting/ScriptingEngine.h"
#include "Utils/FileDialogs.h"
#if defined(BEE_COMPILE_VULKAN)
#include "backends/imgui_impl_vulkan.h"
#endif
#include "imgui.h"
#include <../../Engine/Assets/EmbeddedResources.h>
#include <Core/Move.h>
#include <string_view>

namespace BeeEngine::Editor
{
    Locale::Localization GetSupportedLocale(const Locale::Localization& loadedLocale)
    {
        if (loadedLocale.GetLanguageString() == "en" || loadedLocale.GetLanguageString() == "ru")
        {
            return loadedLocale;
        }
        return Locale::Localization::Default;
    }
    void EditorLayer::OnAttach() noexcept
    {
        SetImGuiFontSize(m_Config.FontSize);
        auto locale = GetSupportedLocale(m_Config.Locale);
        m_EditorLocaleDomain.SetLocale(locale);
        auto localizationFilesPaths =
            Locale::LocalizationGenerator::GetLocalizationFiles(std::filesystem::current_path() / "Localization");
        Locale::LocalizationGenerator::ProcessLocalizationFiles(m_EditorLocaleDomain, localizationFilesPaths);
        m_EditorLocaleDomain.Build();
        SetDefaultImGuiWindowLayoutIfNotPresent();
        ConsoleOutput::SetOutputProvider(&m_Console);
        SetUpMenuBar();
        m_PlayButtonTexture =
            AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::PlayButtonTexture, Locale::Localization::Default);
        m_StopButtonTexture =
            AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::StopButtonTexture, Locale::Localization::Default);

        m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
        m_SceneHierarchyPanel.SetContext(m_ViewPort.CurrentScene());
        m_ContentBrowserPanel.SetContext(m_ViewPort.CurrentScene());
        m_InspectorPanel.SetContext(m_ViewPort.CurrentScene());
        m_ActiveScene = m_ViewPort.CurrentScene();
        m_ViewPort.CurrentScene.valueChanged().connect(
            [this](const Ref<Scene>& newScene)
            {
                m_SceneHierarchyPanel.ClearSelection();
                m_ActiveScene = newScene;
                m_SceneHierarchyPanel.SetContext(newScene);
                m_ContentBrowserPanel.SetContext(newScene);
                m_InspectorPanel.SetContext(newScene);
                m_ActiveScene->OnViewPortResize(m_ViewPort.GetWidth(), m_ViewPort.GetHeight());
            });
    }

    void EditorLayer::OnDetach() noexcept
    {
        if (Project())
        {
            if (m_SceneState == SceneState::Play)
            {
                m_SceneState = SceneState::Edit;
                OnSceneStop();
            }
            ScriptingEngine::Shutdown();
        }
        ConsoleOutput::SetOutputProvider(nullptr);
    }
    void EditorLayer::OnUpdate(FrameData& frameData) noexcept
    {
        BEE_PROFILE_FUNCTION();
        std::unique_lock lock(m_BigLock);
        if (Project() == nullptr)
        {
            return;
        }
        Project()->Update();
        m_SceneHierarchyPanel.Update();
        m_EditorCamera.OnUpdate();
        ScriptingEngine::UpdateAllocatorStatistics();
        switch (m_SceneState)
        {
            case SceneState::Edit:
            {
                if (m_ContentBrowserPanel.NeedsToRegenerateSolution())
                {
                    Project()->RegenerateSolution();
                }
                if (Project()->IsAssemblyReloadPending())
                {
                    ReloadAssembly();
                }
                m_ViewPort.UpdateEditor(m_EditorCamera, m_RenderPhysicsColliders);
                // m_GameBuilder->UpdateAndCompile();
                break;
            }
            case SceneState::Play:
            {
                m_ViewPort.UpdateRuntime(m_RenderPhysicsColliders);
                break;
            }
            case SceneState::Pause:
            {
            }
            case SceneState::Simulate:
            {
                break;
            }
        }
        if (m_ViewPort.IsNewSceneLoaded())
        {
            if (m_SceneState == SceneState::Play)
            {
                m_SceneState = SceneState::Edit;
                OnSceneStop();
            }
            Jobs::Schedule(Jobs::CreateJob(
                [this]()
                {
                    // SaveScene();
                    LoadScene(m_ViewPort.GetSceneHandle());
                }));
        }
        m_FpsCounter.Update();
    }

    void EditorLayer::RenderChooseProjectScreen()
    {
        enum class ProjAction
        {
            Load,
            Create
        };
        auto loadProject = [this](ProjAction action)
        {
            auto projectPath = action == ProjAction::Load ? FileDialogs::OpenFile({"BeeEngine Project", "*.beeproj"})
                                                          : FileDialogs::OpenFolder();
            if (projectPath.IsEmpty())
            {
                return;
            }
            auto name = projectPath.GetFileNameWithoutExtension().AsUTF8();
            if (action == ProjAction::Load)
            {
                projectPath = projectPath.RemoveFileName().AsUTF8();
            }
            {
                std::unique_lock lock(m_BigLock);
                Project.set(CreateScope<ProjectFile>(projectPath, name, &m_EditorAssetManager));
                m_ProjectSettings =
                    CreateScope<ProjectSettings>(*Project(), m_EditorLocaleDomain, m_EditorAssetManager);

                if (action == ProjAction::Load && File::Exists(Project()->AssetRegistryPath()))
                {
                    AssetRegistrySerializer assetRegistrySerializer(
                        &m_EditorAssetManager, Project()->FolderPath(), Project()->GetAssetRegistryID());
                    assetRegistrySerializer.Deserialize(Project()->AssetRegistryPath());
                    std::vector<Path> assetPaths = AssetScanner::GetAllAssetFiles(Project()->FolderPath());
                    for (auto& path : assetPaths)
                    {
                        auto name = path.GetFileNameWithoutExtension().AsUTF8();
                        const auto* handlePtr = m_EditorAssetManager.GetAssetHandleByName(name);
                        if (!handlePtr)
                        {
                            m_EditorAssetManager.LoadAsset(path, {Project.get()->GetAssetRegistryID()});
                        }
                    }
                }
                m_AssetPanel.onAssetRemoved.connect([this](const AssetHandle& handle) { DeleteAsset(handle); });
                Project()->onAssetRemoved.connect([this](const AssetHandle& handle) { DeleteAsset(handle); });
                m_LocalizationPanel = CreateScope<Locale::ImGuiLocalizationPanel>(Project()->GetProjectLocaleDomain(),
                                                                                  Project()->FolderPath());
            }
            auto sceneHandle = Project()->GetLastUsedScene();
            Application::SubmitToMainThread(
                [this, sceneHandle]()
                {
                    Project()->ReloadAndRebuildGameLibrary();
                    Project()->IsAssemblyReloadPending(); // To disable automatic reloading on next
                                                          // frame. We are handling this now
                    SetupGameLibrary();
                    if (sceneHandle != AssetHandle{0, 0})
                    {
                        LoadScene(sceneHandle);
                    }
                    Project()->StartFileWatchers();
                });
        };
        ImGui::Begin(m_EditorLocaleDomain.Translate("projectSelection").c_str());

        if (ImGui::Button(m_EditorLocaleDomain.Translate("loadProject").c_str()))
        {
            auto loadProjectJob =
                Jobs::CreateJob([this, loadProject = BeeMove(loadProject)]() { loadProject(ProjAction::Load); });
            Jobs::Schedule(BeeMove(loadProjectJob));
        }
        if (ImGui::Button(m_EditorLocaleDomain.Translate("newProject").c_str()))
        {
            auto newProjectJob =
                Jobs::CreateJob([this, loadProject = BeeMove(loadProject)]() { loadProject(ProjAction::Create); });
            Jobs::Schedule(BeeMove(newProjectJob));
        }
        ImGui::PushID("editorSettingsButton");
        if (ImGui::Button(m_EditorLocaleDomain.Translate("editorSettings").c_str()))
        {
            m_ShowEditorSettings = !m_ShowEditorSettings;
        }
        ImGui::PopID();
        ImGui::End();
    }

    void EditorLayer::RenderEditor()
    {
        m_MenuBar.Render();
        UIToolbar();
        m_SceneHierarchyPanel.OnGUIRender();
        m_ViewPort.Render(m_EditorCamera);
        m_InspectorPanel.OnGUIRender(m_SceneHierarchyPanel.GetSelectedEntity());
        m_ContentBrowserPanel.OnGUIRender();
        m_AssetPanel.Render();
        m_FpsCounter.Render();
        m_Console.RenderGUI();
        m_LocalizationPanel->Render();
        m_DragAndDrop.ImGuiRender();
        m_ProjectSettings->Render();
        DrawBuildProjectPopup();
        ImGui::Begin(m_EditorLocaleDomain.Translate("settings").c_str());
        ImGui::Checkbox("Render physics colliders", &m_RenderPhysicsColliders);
        if (ImGui::Button("GC Collect"))
        {
            NativeToManaged::GCCollect();
        }
        ImGui::End();
    }

    void EditorLayer::RenderEditorSettings(bool& show)
    {
        if (!show)
        {
            return;
        }
        ImGui::Begin(m_EditorLocaleDomain.Translate("editorSettings").c_str(), &show);

        ImGui::TextUnformatted(m_EditorLocaleDomain.Translate("editorSettings.language").c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##EditorLanguage", m_Config.Locale.GetLanguageString().c_str()))
        {
            if (ImGui::BeginTooltip())
            {
                ImGui::TextUnformatted(m_EditorLocaleDomain.Translate("needsRestartingTooltip").c_str());
                ImGui::EndTooltip();
            }
            for (const auto& locale : m_EditorLocaleDomain.GetLocales())
            {
                if (ImGui::Selectable(locale.c_str()))
                {
                    m_Config.Locale = Locale::Localization{locale};
                }
                if (m_Config.Locale.GetLanguageString() == locale)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
        ImGui::TextWrapped(m_EditorLocaleDomain.Translate("editorSettings.fontSize").c_str());
        // ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::SliderFloat("##fontSizeFloat", &m_Config.FontSize, 12, 100, "%.3f", ImGuiSliderFlags_AlwaysClamp))
        {
            SetImGuiFontSize(m_Config.FontSize);
        }
        ImGui::TextUnformatted(m_EditorLocaleDomain.Translate("editorSettings.thumbnailSize").c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::SliderFloat("##thumbnailSize", &m_Config.ThumbnailSize, 32, 256, "%.3f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::End();
    }

    void EditorLayer::OnGUIRendering() noexcept
    {
        std::unique_lock lock(m_BigLock);
        m_DockSpace.Start();
        if (Project())
        {
            RenderEditor();
        }
        else
        {
            RenderChooseProjectScreen();
        }
        m_UIEditor.Render();
        RenderEditorSettings(m_ShowEditorSettings);
        m_DockSpace.End();
    }

    void EditorLayer::SetImGuiFontSize(float size)
    {
        Application::SubmitToMainThread(
            [size]()
            {
                ImGuiController::CreateFontsWithSize(size);
#if defined(BEE_COMPILE_VULKAN)
                if (Renderer::GetAPI() == RenderAPI::Vulkan)
                {
                    ImGui_ImplVulkan_DestroyFontsTexture();
                    ImGui_ImplVulkan_CreateFontsTexture();
                }
#endif
                BeeCoreTrace("Font size set to {}", size);
            });
    }

    void EditorLayer::OnEvent(EventDispatcher& event) noexcept
    {
        if (Project() == nullptr)
        {
            return;
        }
        if (m_SceneState == SceneState::Play)
        {
            if (m_ViewPort.ShouldHandleEvents())
            {
                m_ViewPort.OnEvent(event);
            }
            return;
        }
        if (m_ViewPort.ShouldHandleEvents())
        {
            m_EditorCamera.OnEvent(event);
            m_ViewPort.OnEvent(event);
        }
        m_SceneHierarchyPanel.OnEvent(event);
        m_DragAndDrop.OnEvent(event);

        event.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) -> bool { return OnKeyPressed(&event); });
        event.Dispatch<WindowResizeEvent>(
            [this](WindowResizeEvent& event) -> bool
            {
                if (!Application::GetInstance().IsMaximized())
                {
                    m_Config.Width = event.GetWidthInPoints();
                    m_Config.Height = event.GetHeightInPoints();
                }
                return false;
            });
    }

    void EditorLayer::SetUpMenuBar()
    {
        MenuBarElement projectMenu = {m_EditorLocaleDomain.Translate("menubar.project")};
        projectMenu.AddChild(
            {m_EditorLocaleDomain.Translate("menubar.project.settings"), [this]() { m_ProjectSettings->Toggle(); }});
        projectMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.build.buildProject"),
                              [this]() { m_ShowBuildProjectPopup = true; }});

        m_MenuBar.AddElement(projectMenu);

        MenuBarElement fileMenu = {m_EditorLocaleDomain.Translate("menubar.file")};

        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.newScene"),
                           [this]()
                           {
                               m_SceneHierarchyPanel.ClearSelection();
                               SaveScene();
                               m_ViewPort.CurrentScene = CreateRef<Scene>();
                           }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.saveScene"), [this]() { SaveScene(); }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.saveSceneAs"), [this]() { SaveSceneAs(); }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.editorSettings"),
                           [this]() { m_ShowEditorSettings = !m_ShowEditorSettings; }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.exit"),
                           []() { BeeEngine::Application::GetInstance().Close(); }});
        m_MenuBar.AddElement(fileMenu);

        MenuBarElement buildMenu = {m_EditorLocaleDomain.Translate("menubar.build")};
        buildMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.build.regenerateVSSolution"),
                            [this]()
                            {
                                if (!Project())
                                {
                                    BeeCoreError("No project loaded");
                                    return;
                                }
                                Project()->RegenerateSolution();
                            }});
        buildMenu.AddChild(
            {m_EditorLocaleDomain.Translate("menubar.build.reloadScripts"), [this]() { ReloadAssembly(); }});
        buildMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.build.rebuildGameLibrary"),
                            [this]()
                            {
                                auto reloadJob =
                                    Jobs::CreateJob([this]() { Project.get()->ReloadAndRebuildGameLibrary(); });
                                Jobs::Schedule(BeeMove(reloadJob));
                            }});
        m_MenuBar.AddElement(buildMenu);

        MenuBarElement viewMenu = {m_EditorLocaleDomain.Translate("menubar.view")};
        viewMenu.AddChild(
            {m_EditorLocaleDomain.Translate("menubar.view.outputConsole"), [this]() { m_Console.Toggle(); }});
        viewMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.view.localization"),
                           [this]() { m_LocalizationPanel->SwitchOpened(); }});
        m_MenuBar.AddElement(viewMenu);
    }

    void EditorLayer::ReloadAssembly()
    {
        const Path tempPath = Project()->FolderPath() / ".beeengine" / "temp.beescene";
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(tempPath);
        m_SceneHierarchyPanel.ClearSelection();
        m_ActiveScene->Clear();
        // Unloads all scenes in order to reload the scripts in Script components
        for (const auto& assetHandle : m_EditorAssetManager.GetAssetHandlesByType(AssetType::Scene))
        {
            if (m_EditorAssetManager.IsAssetLoaded(assetHandle))
            {
                m_EditorAssetManager.UnloadAsset(assetHandle);
            }
        }
        if (ScriptingEngine::IsInitialized())
        {
            ScriptingEngine::ReloadAssemblies();
        }
        serializer.Deserialize(tempPath);
        m_ActiveScene->OnViewPortResize(m_ViewPort.GetWidth(), m_ViewPort.GetHeight());
    }

    void EditorLayer::SetupGameLibrary()
    {
        /*if(!m_GameBuilder)
        {
            m_GameBuilder =
        CreateScope<GameBuilder>(m_ProjectFile->GetProjectPath(),
        ConfigFile::LoadCompilerConfiguration());
        }*/
        // ScriptingEngine::EnableDebugging(); //TODO: enable only in debug
        // builds
        ScriptingEngine::Init(
            [this](AssetHandle handle)
            {
                m_ActiveScene->StopRuntime();
                LoadScene(handle);
                m_ActiveScene->StartRuntime();
            });
        ScriptingEngine::LoadCoreAssembly("libs/BeeEngine.Core.dll");
        Path debugSymbolsPath = Project()->GameAssemblyPath();
        debugSymbolsPath.ReplaceExtension(".pdb");
        ScriptingEngine::LoadGameAssembly(Project()->GameAssemblyPath(), debugSymbolsPath);
        ScriptGlue::Register();
        ScriptingEngine::SetLocaleDomain(Project()->GetProjectLocaleDomain());
        // auto& gameAssembly =
        // ScriptingEngine::LoadGameAssembly(m_ProjectFile->GetProjectPath() /
        // ".beeengine" / "GameLibrary.dll");
        /*if(m_GameLibrary)
        {
            m_GameLibrary->Reload();
            m_NativeScriptFactory->Reload();
        }
        else
        {
            std::string libraryName = "GameLibrary";
            m_GameLibrary =
        CreateScope<DynamicLibrary>(m_ProjectFile->GetProjectPath(),
        libraryName); m_NativeScriptFactory =
        CreateScope<NativeScriptFactory>(m_NativeScriptData);
        }
        if(m_GameLibrary->IsLoaded())
        {
            InitFunction =
        reinterpret_cast<decltype(InitFunction)>(m_GameLibrary->GetFunction("InitGameLibrary"));
            (*InitFunction)(&m_NativeScriptData);
        }
        m_NativeScripts = m_NativeScriptFactory->GetNativeScripts();*/
    }

    void EditorLayer::UIToolbar() noexcept
    {
        auto& colors = ImGui::GetStyle().Colors;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f});
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, {buttonActive.x, buttonActive.y, buttonActive.z, 0.5f});
        ImGui::Begin("##Toolbar",
                     nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
        ImTextureID textureID =
            (ImTextureID)(m_SceneState == SceneState::Edit ? m_PlayButtonTexture->GetGPUResource().GetRendererID()
                                                           : m_StopButtonTexture->GetGPUResource().GetRendererID());
        float size = ImGui::GetWindowHeight() - 4;
        ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - (size * 0.5f));
        if (ImGui::ImageButton(textureID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            if (m_SceneState == SceneState::Edit)
            {
                m_SceneState = SceneState::Play;
            }
            else
            {
                m_SceneState = SceneState::Edit;
            }
            switch (m_SceneState)
            {
                case SceneState::Edit:
                {
                    OnSceneStop();
                    break;
                }
                case SceneState::Play:
                {
                    OnScenePlay();
                    break;
                }
                case SceneState::Pause:
                {
                    OnScenePause();
                    break;
                }
                case SceneState::Simulate:
                {
                    OnSceneSimulate();
                    break;
                }
            }
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::OnScenePlay() noexcept
    {
        m_EditorScene = Scene::Copy(*m_ActiveScene);
        m_ActiveScene->StartRuntime();
    }

    void EditorLayer::OnScenePause() noexcept {}

    void EditorLayer::OnSceneStop() noexcept
    {
        m_ActiveScene->StopRuntime();
        m_ViewPort.CurrentScene = m_EditorScene;
    }

    void EditorLayer::OnSceneSimulate() noexcept {}

    void EditorLayer::LoadScene(const AssetHandle& handle)
    {
        m_ViewPort.CurrentScene = Scene::Copy(AssetManager::GetAsset<Scene>(handle));
        Project.get()->SetLastUsedScene(handle);
    }

    void EditorLayer::SaveSceneAs()
    {
        auto job = Jobs::CreateJob(
            [this, scene = m_ActiveScene]() mutable
            {
                TryToGetFile:
                    auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
                    if (filepath.IsEmpty())
                    {
                        return;
                    }
                    constexpr static auto isSubPath = [](const String& base, const String& potentialSubPath)
                    {
                        // Проверяем, является ли путь potentialSubPath частью base
                        return std::search(
                                   potentialSubPath.begin(), potentialSubPath.end(), base.begin(), base.end()) ==
                               potentialSubPath.begin();
                    };
                    if (!isSubPath(Project()->FolderPath().AsUTF8(), filepath.AsUTF8()))
                    {
                        ShowMessageBox(m_EditorLocaleDomain.Translate("saveScene.invalidPathError.title"),
                                       m_EditorLocaleDomain.Translate("saveScene.invalidPathError.body"),
                                       MessageBoxType::Error);
                        goto TryToGetFile;
                    }
                    if (filepath.GetExtension() != ".beescene")
                    {
                        filepath.ReplaceExtension(".beescene");
                    }
                    SceneSerializer serializer(scene);
                    serializer.Serialize(filepath);
                    Jobs::Counter counter;
                    auto waitForTheAssetToBeAdded =
                        Jobs::CreateJob(counter,
                                        [this, name = filepath.GetFileNameWithoutExtension().AsUTF8()]()
                                        {
                                            const AssetHandle* handlePtr =
                                                m_EditorAssetManager.GetAssetHandleByName(name);
                                            while (!handlePtr)
                                            {
                                                Jobs::this_job::yield();
                                                handlePtr = m_EditorAssetManager.GetAssetHandleByName(name);
                                            }
                                        });
                    Jobs::Schedule(BeeMove(waitForTheAssetToBeAdded));
                    Jobs::WaitForJobsToComplete(counter);
                    AssetHandle newSceneHandle =
                        *m_EditorAssetManager.GetAssetHandleByName(filepath.GetFileNameWithoutExtension().AsUTF8());
                    scene->Handle = newSceneHandle;
                    auto& metadata = m_EditorAssetManager.GetAssetMetadata(newSceneHandle);
                    scene->Name = std::string_view(metadata.Name);
                    Project()->SetLastUsedScene(newSceneHandle);
                    if (Project()->GetStartingSceneName() == "Not available")
                    {
                        Project()->SetStartingScene(newSceneHandle);
                    }
                    Project()->Save();
            });
        Jobs::Schedule(BeeMove(job));
    }

    void EditorLayer::SaveScene()
    {
        SaveAssetRegistry();
        if (!m_EditorAssetManager.IsAssetHandleValid(m_ActiveScene->Handle))
        {
            ShowMessageBox(m_EditorLocaleDomain.Translate("saveScene.currentSceneIsNotSaved.title"),
                           m_EditorLocaleDomain.Translate("saveScene.currentSceneIsNotSaved.body"));
            SaveSceneAs();
            return;
        }
        if (!m_ActiveScene || m_ActiveScene->Handle == AssetHandle{0, 0})
        {
            return;
        }
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(std::get<Path>(m_EditorAssetManager.GetAssetMetadata(m_ActiveScene->Handle).Data));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent* event) noexcept
    {
        bool ctrl = Input::KeyPressed(Key::LeftControl) || Input::KeyPressed(Key::RightControl);
        if (ctrl && Input::KeyPressed(Key::S))
        {
            Application::SubmitToMainThread([this]() { SaveScene(); });
            return true;
        }
        if (Input::KeyPressed(Key::F3))
        {
            m_UIEditor.Toggle();
        }
        return false;
    }

    void EditorLayer::SaveAssetRegistry()
    {
        AssetRegistrySerializer serializer(
            &m_EditorAssetManager, Project()->FolderPath(), Project()->GetAssetRegistryID());
        serializer.Serialize(Project()->AssetRegistryPath());
    }

    void EditorLayer::DeleteAsset(const AssetHandle& handle)
    {
        auto metadata = m_EditorAssetManager.GetAssetMetadata(handle);
        auto type = metadata.Type;
        if (type == AssetType::Texture2D)
        {
            auto view = m_ActiveScene->m_Registry.view<SpriteRendererComponent>();
            for (const auto& entity : view)
            {
                auto& spriteRenderer = view.get<SpriteRendererComponent>(entity);
                if (spriteRenderer.HasTexture && spriteRenderer.TextureHandle == handle)
                {
                    spriteRenderer.HasTexture = false;
                }
            }
        }
        else if (type == AssetType::Font)
        {
            auto view = m_ActiveScene->m_Registry.view<TextRendererComponent>();
            for (const auto& entity : view)
            {
                auto& textComponent = view.get<TextRendererComponent>(entity);
                if (textComponent.FontHandle == handle)
                {
                    textComponent.FontHandle = EngineAssetRegistry::OpenSansRegular;
                }
            }
        }
        m_EditorAssetManager.RemoveAsset(handle);
        if (metadata.Location == AssetLocation::FileSystem && File::Exists(std::get<Path>(metadata.Data)))
        {
            std::filesystem::remove_all(std::get<Path>(metadata.Data).ToStdPath());
        }
        SaveAssetRegistry();
    }

    String EditorLayer::GenerateImGuiINIFile() const
    {
        std::ostringstream result;
        result << R"([Window][DockSpace Demo]
Pos=0,0
Size=1280,720
Collapsed=0)";
        result << '\n' << '\n';
        result << R"([Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0)";
        result << '\n' << '\n';
        result << "[Window][" << m_EditorLocaleDomain.Translate("projectSelection") << R"(]
Pos=0,34
Size=908,686
Collapsed=0
DockId=0x0000000E,0)";
        result << '\n' << '\n';
        result << R"([Window][Profiler]
Pos=910,34
Size=370,486
Collapsed=0
DockId=0x00000007,0)";
        result << '\n' << '\n';
        result << R"([Window][Renderer Statistics]
Pos=910,34
Size=370,486
Collapsed=0
DockId=0x00000007,2)";
        result << '\n' << '\n';
        result << R"([Window][Allocator statistics]
Pos=910,34
Size=370,486
Collapsed=0
DockId=0x00000007,1)";
        result << '\n' << '\n';
        result << R"([Window][##Toolbar]
Pos=371,34
Size=537,31
Collapsed=0
DockId=0x0000000D,0)";
        result << '\n' << '\n';
        result << R"([Window][##Viewport]
Pos=371,67
Size=537,312
Collapsed=0
DockId=0x0000000E,0)";
        result << '\n' << '\n';
        result << "[Window][" << m_EditorLocaleDomain.Translate("sceneHierarchyPanel") << R"(]
Pos=0,34
Size=369,345
Collapsed=0
DockId=0x0000000B,0)";
        result << '\n' << '\n';
        result << "[Window][" << m_EditorLocaleDomain.Translate("inspector") << R"(]
Pos=910,34
Size=370,486
Collapsed=0
DockId=0x00000007,3)";
        result << '\n' << '\n';
        result << "[Window][" << m_EditorLocaleDomain.Translate("contentBrowserPanel") << R"(]
Pos=0,381
Size=536,339
Collapsed=0
DockId=0x00000005,0)";
        result << '\n' << '\n';
        result << "[Window][" << m_EditorLocaleDomain.Translate("assetPanel") << R"(]
Pos=538,381
Size=370,339
Collapsed=0
DockId=0x00000006,0)";
        result << '\n' << '\n';
        result << R"([Window][FPS]
Pos=910,623
Size=370,97
Collapsed=0
DockId=0x0000000A,0)";
        result << '\n' << '\n';
        result << R"([Window][Output Console]
Pos=0,381
Size=536,339
Collapsed=0
DockId=0x00000005,1)";
        result << '\n' << '\n';
        result << "[Window][" << m_EditorLocaleDomain.Translate("settings") << R"(]
Pos=910,522
Size=370,99
Collapsed=0
DockId=0x00000009,0)";
        result << '\n' << '\n';
        result << R"([Docking][Data]
DockSpace         ID=0x3BC79352 Window=0x4647B76E Pos=0,34 Size=1280,686 Split=X Selected=0xD04A4B96
  DockNode        ID=0x00000002 Parent=0x3BC79352 SizeRef=908,686 Split=Y Selected=0xD04A4B96
    DockNode      ID=0x00000003 Parent=0x00000002 SizeRef=908,345 Split=X
      DockNode    ID=0x0000000B Parent=0x00000003 SizeRef=369,345 Selected=0xCB122D77
      DockNode    ID=0x0000000C Parent=0x00000003 SizeRef=537,345 Split=Y Selected=0x064DAA9B
        DockNode  ID=0x0000000D Parent=0x0000000C SizeRef=537,31 HiddenTabBar=1 Selected=0x766B88B3
        DockNode  ID=0x0000000E Parent=0x0000000C SizeRef=537,312 CentralNode=1 HiddenTabBar=1 Selected=0x064DAA9B
    DockNode      ID=0x00000004 Parent=0x00000002 SizeRef=908,339 Split=X Selected=0x93769EF8
      DockNode    ID=0x00000005 Parent=0x00000004 SizeRef=536,339 Selected=0x98E83348
      DockNode    ID=0x00000006 Parent=0x00000004 SizeRef=370,339 Selected=0x53414144
  DockNode        ID=0x00000001 Parent=0x3BC79352 SizeRef=370,686 Split=Y Selected=0x1B782AF8
    DockNode      ID=0x00000007 Parent=0x00000001 SizeRef=370,486 Selected=0x1B782AF8
    DockNode      ID=0x00000008 Parent=0x00000001 SizeRef=370,198 Split=Y Selected=0x54723243
      DockNode    ID=0x00000009 Parent=0x00000008 SizeRef=370,99 Selected=0x54723243
      DockNode    ID=0x0000000A Parent=0x00000008 SizeRef=370,97 Selected=0x6108FA95
)";
        result << '\n' << '\n';
        return String{result.str()};
    }

    void EditorLayer::SetDefaultImGuiWindowLayoutIfNotPresent()
    {
        auto iniPath = std::filesystem::current_path() / "imgui.ini";
        if (std::filesystem::exists(iniPath))
        {
            // Check if current locale is correct
            auto content = File::ReadFile(iniPath);
            if (content.contains(m_EditorLocaleDomain.Translate("sceneHierarchyPanel")))
            {
                return;
            }
        }
        File::WriteFile(iniPath, GenerateImGuiINIFile());
        BeeCoreTrace("Setting default Editor layout");
        // TODO: make a generation of imgui.ini based on locale or make it
        // independant from locale
    }
    void EditorLayer::DrawBuildProjectPopup()
    {
        if (!m_ShowBuildProjectPopup)
        {
            return;
        }
        ImGui::OpenPopup(m_EditorLocaleDomain.Translate("buildProject").c_str());
        ImGui::BeginPopupModal(
            m_EditorLocaleDomain.Translate("buildProject").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        static BuildProjectOptions options;
        static String outputPath;
        ImGui::TextUnformatted(m_EditorLocaleDomain.Translate("buildProject.buildtype").c_str());
        ImGui::RadioButton("Debug",
                           reinterpret_cast<int*>(&options.BuildType),
                           static_cast<int>(BuildProjectOptions::BuildType::Debug));
        ImGui::SameLine();
        ImGui::RadioButton("Release",
                           reinterpret_cast<int*>(&options.BuildType),
                           static_cast<int>(BuildProjectOptions::BuildType::Release));

        // Choose a default locale from the project locale domain
        ProjectSettings::DropMenuChooseDefaultLocale(*Project(), m_EditorLocaleDomain);
        ImGui::TextUnformatted(
            (m_EditorLocaleDomain.Translate("buildProject.startingScene") + ": " + Project()->GetStartingSceneName())
                .c_str());

        ImGui::TextUnformatted(m_EditorLocaleDomain.Translate("buildProject.outputPath").c_str());
        ImGui::SameLine();
        if (ImGui::InputText("##", &outputPath), ImGuiInputTextFlags_ReadOnly)
        {
            options.OutputPath = outputPath;
        }
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            auto getOutputPathJob = Jobs::CreateJob(
                []()
                {
                    Path chosenPath = FileDialogs::OpenFolder();
                    if (chosenPath.IsEmpty())
                    {
                        return;
                    }
                    outputPath = chosenPath.AsUTF8();
                    options.OutputPath = std::move(chosenPath);
                });
            Jobs::Schedule(BeeMove(getOutputPathJob));
        }
        static String customError;
        auto areAllFieldsFilled = [this]() -> bool
        {
            customError.clear();
            bool isOutputPathEmpty = options.OutputPath.IsEmpty();
            bool isOutputPathValid = false;
            bool isDefaultLocaleValid = false;
            if (!isOutputPathEmpty)
            {
                isOutputPathValid = File::Exists(options.OutputPath) && File::IsDirectory(options.OutputPath);
            }
            if (!isOutputPathValid)
            {
                customError += '\n' + m_EditorLocaleDomain.Translate("buildProject.outputPath.invalidError");
            }
            for (const auto& locale : Project()->GetProjectLocaleDomain().GetLocales())
            {
                if (locale == options.DefaultLocale.GetLanguageString())
                {
                    isDefaultLocaleValid = true;
                    break;
                }
            }
            if (!isDefaultLocaleValid)
            {
                customError += '\n' + m_EditorLocaleDomain.Translate("buildProject.defaultLocale.invalidError");
            }
            bool isEverythingFilled = !isOutputPathEmpty && isOutputPathValid && isDefaultLocaleValid;
            return isEverythingFilled;
        };
        if (options.DefaultLocale != Project()->DefaultLocale())
        {
            options.DefaultLocale = Project()->DefaultLocale();
        }
        bool disabled = !areAllFieldsFilled();
        if (disabled)
        {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button(m_EditorLocaleDomain.Translate("buildProject.build").c_str()))
        {
            Project()->BuildProject(options);
            ImGui::CloseCurrentPopup();
            m_ShowBuildProjectPopup = false;
        }
        if (disabled)
        {
            ImGui::EndDisabled();
            if (ImGui::BeginItemTooltip())
            {
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(
                    (m_EditorLocaleDomain.Translate("buildProject.build.tooltip") + "\n" + customError).c_str());
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(m_EditorLocaleDomain.Translate("cancel").c_str()))
        {
            ImGui::CloseCurrentPopup();
            m_ShowBuildProjectPopup = false;
        }
        ImGui::EndPopup();
    }
} // namespace BeeEngine::Editor
