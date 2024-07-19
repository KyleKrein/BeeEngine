//
// Created by alexl on 20.07.2023.
//

#include "ProjectFile.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetRegistrySerializer.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Logging/Log.h"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"
#include "Locale/LocalizationGenerator.h"
#include "Utils/Commands.h"
#include "VSProjectGeneration.h"
#include <Core/GameConfig.h>
#include <Serialization/YAMLHelper.h>
#include <filesystem>
#include <fstream>

namespace BeeEngine::Editor
{
    const String dotNetVersion = "net8.0";
    ProjectFile::ProjectFile(const Path& projectPath,
                             const String& projectName,
                             EditorAssetManager* assetManager) noexcept
        : Name(projectName),
          FolderPath(projectPath),
          FilePath(makeBoundProperty(FolderPath / (Name + ".beeproj"))),
          AssetRegistryPath(makeBoundProperty(FolderPath / (Name + ".beeassetregistry"))),
          m_AssetManager(assetManager),
          m_ProjectLocaleDomain(projectName)
    {
        Name.valueAboutToChange().connect(
            [this](const String& oldName, const String& newName)
            {
                std::filesystem::remove(FilePath.get().ToStdPath());
                std::filesystem::rename(AssetRegistryPath.get().ToStdPath(),
                                        (FolderPath.get() / (newName + ".beeassetregistry")).ToStdPath());
            });
        Name.valueChanged().connect(
            [this](const String& newName)
            {
                ResourceManager::ProjectName = newName;
                Save();
            });
        DefaultLocale.valueChanged().connect([this](const auto& newLocale) { Save(); });
        BeeCoreTrace("ProjectName: {0}", Name.get());
        BeeCoreTrace("ProjectPath: {0}", FolderPath.get().AsUTF8());
        ResourceManager::ProjectName = Name.get();
        LoadLocalizationFiles();
        if (!File::Exists(FolderPath.get() / ".beeengine"))
        {
            File::CreateDirectory(FolderPath.get() / ".beeengine");
        }
        if (!File::Exists(FolderPath.get() / ".beeengine" / "build"))
        {
            GameAssemblyPath = FolderPath.get() / ".beeengine" / "build";
            File::CreateDirectory(GameAssemblyPath.get());
            GameAssemblyPath = GameAssemblyPath.get() / dotNetVersion;
            File::CreateDirectory(GameAssemblyPath.get());
        }
        else
            GameAssemblyPath = FolderPath.get() / ".beeengine" / "build" / dotNetVersion;
        GameAssemblyPath = GameAssemblyPath.get() / "GameLibrary.dll";
        std::filesystem::copy_file(std::filesystem::current_path() / "libs" / "BeeEngine.Core.dll",
                                   FolderPath.get().ToStdPath() / ".beeengine" / "BeeEngine.Core.dll",
                                   std::filesystem::copy_options::overwrite_existing);
        if (!File::Exists(FilePath.get()))
        {
        init:
            Save();
            Path currentConfigFile = std::filesystem::current_path() / "projects.cfg";
            if (!File::Exists(currentConfigFile))
            {
                /*std::ofstream fout(currentConfigFile, std::ios::out);
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "Projects";
                out << YAML::BeginSeq;
                out << YAML::BeginMap;
                out << YAML::Key << "ProjectName" << YAML::Value << m_ProjectName;
                out << YAML::Key << "ProjectPath" << YAML::Value << m_ProjectPath.AsUTF8();
                out << YAML::EndMap;
                out << YAML::EndSeq;
                out << YAML::EndMap;
                fout << "LastProject: " << m_ProjectName;
                fout.close();*/
            }
            else
            {
                /*YAML::Node data = YAML::LoadFile(currentConfigFile.string());
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "ProjectName" << YAML::Value << m_ProjectName;
                out << YAML::Key << "ProjectPath" << YAML::Value << m_ProjectPath.string();
                out << YAML::EndMap;
                data["Projects"].push_back(YAML::Load(out.c_str()));
                std::ofstream fout(currentConfigFile);
                fout << data;*/
            }
            File::CreateDirectory(FolderPath.get() / "Assets");
            File::CreateDirectory(FolderPath.get() / "Scenes");

            RegenerateSolution();
            return;
        }
        else
        {
            RegenerateSolution();
        }
        {
            std::ifstream ifs(FilePath.get().ToStdPath());
            YAML::Node data = YAML::Load(ifs);
            ifs.close();
            if (!data["ProjectName"])
            {
                goto init;
            }
            m_AssetRegistryID = data["Asset Registry ID"].as<uint64_t>();
            if (data["DefaultLocale"])
            {
                DefaultLocale = Locale::Localization(String{data["DefaultLocale"].as<std::string>()});
            }
            if (data["StartingScene"])
            {
                SetStartingScene(data["StartingScene"].as<AssetHandle>());
            }
        }
        {
            Path userConfigPath = FolderPath.get() / ".beeengine" / "usersettings.cfg";
            if (File::Exists(userConfigPath))
            {
                std::ifstream ifs(userConfigPath.ToStdPath());
                YAML::Node data = YAML::Load(ifs);
                ifs.close();
                SetLastUsedScene(data["LastUsedScene"].as<AssetHandle>());
            }
        }
    }

    void ProjectFile::Save()
    {
        {
            YAML::Emitter out;
            out << YAML::BeginMap;

            out << YAML::Key << "ProjectName" << YAML::Value << Name.get().c_str();
            out << YAML::Key << "Asset Registry ID" << YAML::Value << (uint64_t)m_AssetRegistryID;
            out << YAML::Key << "DefaultLocale" << YAML::Value << DefaultLocale.get().GetLanguageString().c_str();
            out << YAML::Key << "StartingScene" << YAML::Value << m_StartingScene;
            out << YAML::EndMap;

            File::WriteFile(FilePath.get(), String{out.c_str()});
        }

        {
            YAML::Emitter out;
            out << YAML::BeginMap;
            out << YAML::Key << "LastUsedScene" << YAML::Value << m_LastUsedScene;
            out << YAML::EndMap;
            File::WriteFile(FolderPath.get() / ".beeengine" / "usersettings.cfg", String{out.c_str()});
        }
    }

    void ProjectFile::SetLastUsedScene(const AssetHandle& handle)
    {
        m_LastUsedScene = handle;
        Save();
    }
    static String FixProjectName(const String& name)
    {
        String result = name;
        for (auto& c : result)
        {
            if (c == ' ')
            {
                c = '_';
            }
        }
        return result;
    }

    void ProjectFile::RegenerateSolution()
    {
        auto sources = VSProjectGeneration::GetSourceFiles(FolderPath.get());
        VSProjectGeneration::GenerateProject(FolderPath.get(), sources, Name.get());
    }

    std::vector<std::pair<OSPlatform, Path>> ProjectFile::CheckForAvailablePlatforms()
    {
        std::vector<std::pair<OSPlatform, Path>> result;
        const Path windowsPath = std::filesystem::current_path() / "Platforms" / "Windows";
        if (File::Exists(windowsPath))
            result.emplace_back(OSPlatform::Windows, windowsPath);
        const Path linuxPath = std::filesystem::current_path() / "Platforms" / "Linux";
        if (File::Exists(linuxPath))
            result.emplace_back(OSPlatform::Linux, linuxPath);
        const Path macPath = std::filesystem::current_path() / "Platforms" / "MacOS";
        if (File::Exists(macPath))
            result.emplace_back(OSPlatform::Mac, macPath);
        const Path iosPath = std::filesystem::current_path() / "Platforms" / "iOS";
        if (File::Exists(iosPath))
            result.emplace_back(OSPlatform::iOS, iosPath);
        const Path androidPath = std::filesystem::current_path() / "Platforms" / "Android";
        if (File::Exists(androidPath))
            result.emplace_back(OSPlatform::Android, androidPath);
        return result;
    }

    class BuildProjectAssetManager : public IAssetManager
    {
    public:
        Ref<Asset> GetAssetRef(AssetHandle handle) const { return nullptr; }
        Asset* GetAsset(AssetHandle handle) const { return nullptr; }
        void LoadAsset(gsl::span<byte> data, AssetHandle handle, const String& name, AssetType type) {}
        void LoadAsset(const Path& path, AssetHandle handle) {}
        void UnloadAsset(AssetHandle handle) {}

        bool IsAssetHandleValid(AssetHandle handle) const { return false; }
        bool IsAssetLoaded(AssetHandle handle) const { return false; }

        AssetRegistry& GetAssetRegistry() { return m_AssetRegistry; }

    private:
        AssetRegistry m_AssetRegistry;
    };

    void ProjectFile::BuildProject(const BuildProjectOptions& options)
    {
        const Path libraryOutputPath = FolderPath.get().AsUTF8() + "/.beeengine/build/" + ToString(options.BuildType);
        RunCommand("dotnet build " + FolderPath.get().AsUTF8() + "/" + Name.get() + ".sln --configuration " +
                   ToString(options.BuildType) + " --output " + libraryOutputPath.AsUTF8());
        const Path gameLibraryPath = libraryOutputPath / "GameLibrary.dll";
        const auto outputPath = options.OutputPath.ToStdPath();
        std::filesystem::remove_all(outputPath);
        std::filesystem::create_directory(outputPath);

        const std::vector<std::pair<OSPlatform, Path>> availablePlatforms = CheckForAvailablePlatforms();
        for (const auto& [platform, pathToTemplate] : availablePlatforms)
        {
            const auto platformOutputPath = outputPath / ToString(platform).c_str();
            std::filesystem::create_directory(platformOutputPath);
            std::filesystem::copy(pathToTemplate.ToStdPath(),
                                  platformOutputPath,
                                  std::filesystem::copy_options::recursive |
                                      std::filesystem::copy_options::overwrite_existing);
            Path gameFilesPath;
            switch (platform)
            {
                case OSPlatform::Windows:
                    gameFilesPath = BuildWindowsGame(gameLibraryPath, platformOutputPath);
                    break;
                case OSPlatform::Mac:
                    gameFilesPath = BuildMacOSGame(gameLibraryPath, platformOutputPath);
                    break;
                default:
                    BeeCoreWarn("Cannot build for platform {0}", platform);
                    continue;
            }
            const Path gameConfigPath = gameFilesPath / "Game.cfg";
            const Path assetRegistryPath = gameFilesPath / (Name.get() + ".beeassetregistry");

            const Path assetPath = gameFilesPath / "Assets";
            const Path scenePath = assetPath / "Scenes";
            const Path texturePath = assetPath / "Textures";
            const Path localePath = assetPath / "Localization";
            const Path prefabPath = assetPath / "Prefabs";
            const Path fontPath = assetPath / "Fonts";

            File::CreateDirectory(assetPath);
            File::CreateDirectory(scenePath);
            File::CreateDirectory(texturePath);
            File::CreateDirectory(localePath);
            File::CreateDirectory(prefabPath);
            File::CreateDirectory(fontPath);

            AssetRegistry assetRegistry;
            for (const auto& [registryUuid, assetMap] : m_AssetManager->GetAssetRegistry())
            {
                if (registryUuid != m_AssetRegistryID)
                {
                    continue; // TODO add support for multiple asset registries
                }
                for (const auto& [uuid, metadata] : assetMap)
                {
                    if (metadata.Location == AssetLocation::Embedded)
                    {
                        continue;
                    }
                    AssetMetadata meta = metadata;
                    auto choosePath = [&]() -> const Path&
                    {
                        switch (meta.Type)
                        {
                            case AssetType::Scene:
                                return scenePath;
                            case AssetType::Texture2D:
                                return texturePath;
                            case AssetType::Prefab:
                                return prefabPath;
                            case AssetType::Font:
                                return fontPath;
                            default:
                                BeeCoreWarn("Unsupported asset type in project build{0}", meta.Type);
                                return assetPath;
                        }
                    };
                    const Path assetOutputPath = choosePath() / std::get<Path>(meta.Data).GetFileName();
                    File::CopyFile(std::get<Path>(meta.Data), assetOutputPath);
                    meta.Data = assetOutputPath;
                    assetRegistry[registryUuid].emplace(uuid, std::move(meta));
                }
            }
            BuildProjectAssetManager assetManager;
            assetManager.GetAssetRegistry() = std::move(assetRegistry);
            AssetRegistrySerializer serializer(&assetManager, gameFilesPath, m_AssetRegistryID);
            serializer.Serialize(assetRegistryPath);

            for (const auto& entry : std::filesystem::recursive_directory_iterator(FolderPath.get().ToStdPath()))
            {
                auto extension = entry.path().extension();
                if (extension == ".yaml")
                {
                    File::CopyFile(entry.path(), localePath / entry.path().filename());
                    continue;
                }
            }

            GameConfig config;
            config.Name = Name.get();
            config.DefaultLocale = options.DefaultLocale;
            config.StartingScene = m_StartingScene;
            config.Serialize(gameConfigPath);
        }
        std::filesystem::remove_all(libraryOutputPath.ToStdPath());
        BeeCoreInfo("Project {0} built successfully in {1} mode!", Name.get(), options.BuildType);
    }

    Path ProjectFile::BuildWindowsGame(const Path& gameLibraryPath, const Path& outputDirectory)
    {
        if (!File::Exists(outputDirectory / "libs"))
        {
            File::CreateDirectory(outputDirectory / "libs");
        }
        File::CopyFile(gameLibraryPath, outputDirectory / "libs" / "GameLibrary.dll");
        std::filesystem::rename((outputDirectory / "GameRuntime.exe").ToStdPath(),
                                (outputDirectory / (Name.get() + ".exe")).ToStdPath());
        return outputDirectory;
    }
    Path ProjectFile::BuildMacOSGame(const Path& gameLibraryPath, const Path& outputDirectory)
    {
        const Path runtimePath = outputDirectory / (Name.get() + ".app");
        std::filesystem::rename((outputDirectory / "GameRuntime.app").ToStdPath(), runtimePath.ToStdPath());
        const Path contentsPath = runtimePath / "Contents";
        // std::filesystem::rename((contentsPath / "MacOS" / "GameRuntime").ToStdPath(), (contentsPath / "MacOS" /
        // m_ProjectName).ToStdPath());
        // TODO: add info.plist generation

        Path resourcesPath = contentsPath / "Resources";
        if (!File::Exists(resourcesPath / "libs"))
        {
            File::CreateDirectory(resourcesPath / "libs");
        }
        File::CopyFile(gameLibraryPath, resourcesPath / "libs" / "GameLibrary.dll");
        return resourcesPath;
    }

    void ProjectFile::OnAppAssemblyFileSystemEvent(const Path& path, const FileWatcher::Event changeType)
    {
        if (!m_AssemblyReloadPending &&
            (changeType == FileWatcher::Event::Modified || changeType == FileWatcher::Event::Added))
        {
            m_AssemblyReloadPending = true;
        }
    }

    void ProjectFile::Update() noexcept
    {
        if (m_MustReload)
        {
            m_MustReload = false;
            ReloadAndRebuildGameLibrary();
        }
    }

    Generator<const AssetMetadata&> ProjectFile::GetAssetsForDirectory(const Path& directory)
    {
        auto& assets = m_AssetManager->GetAssetRegistry().at(m_AssetRegistryID);
        for (const auto& [uuid, metadata] : assets)
        {
            if (metadata.Location == AssetLocation::Embedded)
            {
                continue;
            }
            if (std::get<Path>(metadata.Data).AsUTF8().contains(directory.AsUTF8()))
            {
                co_yield metadata;
            }
        }
    }

    void ProjectFile::OnAssetFileSystemEvent(const Path& path, FileWatcher::Event changeType)
    {
        Path p = path;
        if (p.IsRelative())
            p = FolderPath.get() / p;
        if (p.AsUTF8().contains(".git") || p.AsUTF8().contains(".beeengine"))
        {
            return;
        }

        if (File::IsDirectory(p) || p.GetExtension().IsEmpty())
        {
            /*if (p.AsUTF8().contains("bin") || p.AsUTF8().contains("obj"))
            {
                return;
            }*/
            if (changeType == FileWatcher::Event::Modified)
            {
                return;
            }
            static Path oldDirPath;
            if (changeType == FileWatcher::Event::RenamedOldName)
            {
                oldDirPath = p;
                return;
            }
            if (changeType == FileWatcher::Event::RenamedNewName)
            {
                for (const auto& metadata : GetAssetsForDirectory(oldDirPath))
                {
                    OnAssetFileSystemEvent(std::get<Path>(metadata.Data), FileWatcher::Event::RenamedOldName);
                    Path newPath = p / std::get<Path>(metadata.Data).GetRelativePath(oldDirPath);
                    OnAssetFileSystemEvent(newPath, FileWatcher::Event::RenamedNewName);
                }
                return;
            }
            for (const auto& metadata : GetAssetsForDirectory(p))
            {
                OnAssetFileSystemEvent(std::get<Path>(metadata.Data), changeType);
            }
            return;
        }
        if (ResourceManager::IsScriptExtension(p.GetExtension()))
        {
            HandleChangedScriptFile(path, changeType);
            return;
        }
        if (!ResourceManager::IsAssetExtension(p.GetExtension()))
            return;
        String name = p.GetFileNameWithoutExtension();
        const AssetHandle* handlePtr = m_AssetManager->GetAssetHandleByName(name);
        bool changed = false;
        if (!handlePtr and changeType != FileWatcher::Event::RenamedNewName)
        {
            if (changeType == FileWatcher::Event::Added)
            {
                m_AssetManager->LoadAsset(p, {m_AssetRegistryID});
                changed = true;
            }
            goto CHANGED;
        }
        {
            static Path oldNameOnRenamed;
            if (changeType == FileWatcher::Event::RenamedNewName)
            {
                BeeCoreTrace("Renamed {0} to {1}", oldNameOnRenamed.AsUTF8(), p.AsUTF8());
                Application::SubmitToMainThread(
                    [oldPath = oldNameOnRenamed, newPath = p, this]()
                    {
                        // Renaming
                        auto* handlePtr =
                            m_AssetManager->GetAssetHandleByName(oldPath.GetFileNameWithoutExtension().AsUTF8());
                        BeeExpects(handlePtr);
                        AssetHandle handle = *handlePtr;
                        m_AssetManager->RemoveAsset(handle);
                        m_AssetManager->LoadAsset(newPath, handle);
                        AssetRegistrySerializer serializer(m_AssetManager, FolderPath.get(), m_AssetRegistryID);
                        serializer.Serialize(AssetRegistryPath.get());
                    });
                return;
            }
            AssetHandle handle = *handlePtr;

            switch (changeType)
            {
                case FileWatcher::Event::Added:
                    Application::SubmitToMainThread(
                        [this, p, handle]()
                        {
                            m_AssetManager->LoadAsset(p, handle);
                            AssetRegistrySerializer serializer(m_AssetManager, FolderPath.get(), m_AssetRegistryID);
                            serializer.Serialize(AssetRegistryPath.get());
                        });
                    changed = true;
                    break;
                case FileWatcher::Event::Removed:
                    Application::SubmitToMainThread(
                        [this, handle]()
                        {
                            if (!m_AssetManager->IsAssetHandleValid(handle))
                                return;
                            onAssetRemoved.emit(handle);
                        });
                    break;
                case FileWatcher::Event::Modified:
                    if (m_AssetManager->IsAssetLoaded(handle))
                    {
                        Application::SubmitToMainThread(
                            [this, handle]()
                            {
                                if (m_AssetManager->IsAssetLoaded(handle))
                                    m_AssetManager->UnloadAsset(handle);
                            });
                    }
                    break;
                case FileWatcher::Event::RenamedOldName:
                    oldNameOnRenamed = p;
                    break;
            }
        }
    CHANGED:
        if (changed)
        {
            Application::SubmitToMainThread(
                [this]()
                {
                    AssetRegistrySerializer serializer(m_AssetManager, FolderPath.get(), m_AssetRegistryID);
                    serializer.Serialize(AssetRegistryPath.get());
                });
        }
    }

    void ProjectFile::LoadLocalizationFiles()
    {
        auto paths = Locale::LocalizationGenerator::GetLocalizationFiles(FolderPath.get());
        Locale::LocalizationGenerator::ProcessLocalizationFiles(m_ProjectLocaleDomain, paths);
        m_ProjectLocaleDomain.Build();
    }

    void ProjectFile::HandleChangedScriptFile(const Path& path, FileWatcher::Event event)
    {
        if (event == FileWatcher::Event::RenamedOldName || event == FileWatcher::Event::Added ||
            path.GetFileName().AsUTF8().contains("AssemblyInfo.cs"))
        {
            return;
        }
        if (!Application::GetInstance().IsFocused())
        {
            m_MustReload = true;
            return;
        }
        ReloadAndRebuildGameLibrary();
    }

    void ProjectFile::ReloadAndRebuildGameLibrary()
    {
        RegenerateSolution();
        if (m_AppAssemblyFileWatcher)
        {
            m_AppAssemblyFileWatcher->Stop();
        }
        RunCommand("dotnet build \"" + FolderPath.get().AsUTF8() + "/" + Name.get() + ".sln\" --configuration Debug");
        if (m_AppAssemblyFileWatcher)
        {
            m_AppAssemblyFileWatcher->Start();
        }
        m_AssemblyReloadPending = true;
    }

    void ProjectFile::StartFileWatchers()
    {
        if (!m_AppAssemblyFileWatcher)
        {
            m_AppAssemblyFileWatcher = FileWatcher::Create(GameAssemblyPath.get(),
                                                           [this](const Path& path, FileWatcher::Event event)
                                                           { OnAppAssemblyFileSystemEvent(path, event); });
        }
        else
        {
            m_AppAssemblyFileWatcher->Start();
        }
        if (!m_AssetFileWatcher)
        {
            m_AssetFileWatcher = FileWatcher::Create(FolderPath.get(),
                                                     [this](const Path& path, FileWatcher::Event event)
                                                     { OnAssetFileSystemEvent(path, event); });
        }
        else
        {
            m_AssetFileWatcher->Start();
        }
    }

    void ProjectFile::StopFileWatchers()
    {
        BeeExpects(m_AppAssemblyFileWatcher and m_AssetFileWatcher);
        m_AppAssemblyFileWatcher->Stop();
        m_AssetFileWatcher->Stop();
    }
} // namespace BeeEngine::Editor
