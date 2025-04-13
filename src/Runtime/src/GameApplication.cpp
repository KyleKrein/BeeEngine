#include "GameApplication.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/Move.h"
#include "GameLayer.h"
#include "Scene/Scene.h"
#include "Scripting/ScriptingEngine.h"
namespace BeeEngine::Runtime
{
    GameApplication::GameApplication(RuntimeConfiguration config)
        : Application(config.ApplicationProperties),
          m_Config(std::move(config)),
          m_LocaleDomain(m_Config.GameConfig.Name)
    {
        LoadLocalizationFiles();
        LoadAssetRegistry();
        InitializeScripting();
        LoadStartingScene();

        FrameBufferPreferences preferences;
        auto& window = *WindowHandler::GetInstance();
        preferences.Width = window.GetWidthInPixels();
        preferences.Height = window.GetHeightInPixels();
        preferences.SwapChainTarget = false;
        preferences.Attachments = {
            FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RedInteger, FrameBufferTextureFormat::Depth24};

        preferences.Attachments.Attachments[1].TextureUsage = FrameBufferTextureUsage::CPUAndGPU; // RedInteger

        m_FrameBuffer = FrameBuffer::Create(preferences);

        m_GameLayer = CreateRef<GameLayer>(m_ActiveScene, m_FrameBuffer, m_LocaleDomain);

        PushLayer(m_GameLayer);
    }
    void GameApplication::LoadLocalizationFiles()
    {
        auto paths = Locale::LocalizationGenerator::GetLocalizationFiles(std::filesystem::current_path() / "Assets" /
                                                                         "Localization");
        Locale::LocalizationGenerator::ProcessLocalizationFiles(m_LocaleDomain, paths);
        m_LocaleDomain.Build();

        bool useDefaultLocale = true;
        Locale::Localization userLocale = Locale::GetSystemLocale();
        for (const auto& locale : m_LocaleDomain.GetLocales())
        {
            if (locale == userLocale.GetLanguageString())
            {
                useDefaultLocale = false;
                m_LocaleDomain.SetLocale(userLocale);
                break;
            }
        }
        if (useDefaultLocale)
        {
            m_LocaleDomain.SetLocale(m_Config.GameConfig.DefaultLocale);
        }
    }
    void GameApplication::InitializeScripting()
    {
        ScriptingEngine::Init(
            [this](AssetHandle handle)
            {
                m_ActiveScene->StopRuntime();
                m_ActiveScene = std::move(Scene::Copy(AssetManager::GetAsset<Scene>(handle)));
                m_GameLayer->SetScene(m_ActiveScene);
                m_ActiveScene->StartRuntime();
            });
        ScriptingEngine::LoadCoreAssembly("libs/BeeEngine.Core.dll");
        ScriptingEngine::LoadGameAssembly("libs/GameLibrary.dll");
        ScriptGlue::Register();
        ScriptingEngine::SetLocaleDomain(m_LocaleDomain);
    }
    void GameApplication::LoadAssetRegistry()
    {
        AssetRegistrySerializer serializer(&m_AssetManager, std::filesystem::current_path(), {});
        serializer.Deserialize(std::filesystem::current_path() /
                               (m_Config.GameConfig.Name + ".beeassetregistry").c_str());
    }
    void GameApplication::LoadStartingScene()
    {
        m_ActiveScene = Scene::Copy(AssetManager::GetAsset<Scene>(m_Config.GameConfig.StartingScene));
    }
    GameApplication::~GameApplication()
    {
        ScriptingEngine::Shutdown();
    }
} // namespace BeeEngine::Runtime
