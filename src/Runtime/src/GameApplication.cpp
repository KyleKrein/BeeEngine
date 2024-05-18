#include "GameApplication.h"
#include "GameLayer.h"
namespace BeeEngine::Runtime
{
    GameApplication::GameApplication(RuntimeConfiguration config)
        : Application(config.ApplicationProperties),
          m_Config(std::move(config)),
          m_LocaleDomain(m_Config.GameConfig.Name)
    {
        LoadLocalizationFiles();
        InitializeScripting();
        LoadAssetRegistry();
        LoadStartingScene();

        FrameBufferPreferences preferences;
        auto& window = *WindowHandler::GetInstance().get();
        preferences.Width = window.GetWidthInPixels();
        preferences.Height = window.GetHeightInPixels();
        preferences.SwapChainTarget = true;
        preferences.Attachments = {
            FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RedInteger, FrameBufferTextureFormat::Depth24};

        preferences.Attachments.Attachments[1].TextureUsage = FrameBufferTextureUsage::CPUAndGPU; // RedInteger

        m_FrameBuffer = FrameBuffer::Create(preferences);

        PushLayer(CreateRef<GameLayer>(m_ActiveScene, m_FrameBuffer, m_LocaleDomain));
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
        ScriptingEngine::Init();
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
        m_ActiveScene = CreateRef<Scene>();
        SceneSerializer serializer(m_ActiveScene);
        serializer.Deserialize(Path(std::filesystem::current_path()) / m_Config.GameConfig.StartingScene);
    }
    GameApplication::~GameApplication()
    {
        ScriptingEngine::Shutdown();
    }
} // namespace BeeEngine::Runtime
