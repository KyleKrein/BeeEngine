#pragma once
#include "GameLayer.h"
#include <BeeEngine.h>
#include <Core/AssetManagement/AssetRegistrySerializer.h>
#include <Core/AssetManagement/EditorAssetManager.h>
#include <Core/GameConfig.h>
#include <Locale/Locale.h>
#include <Locale/LocalizationGenerator.h>
#include <Renderer/SceneRenderer.h>
#include <Scene/SceneSerializer.h>
#include <Scripting/ScriptGlue.h>
#include <Scripting/ScriptingEngine.h>


namespace BeeEngine::Runtime
{
    struct RuntimeConfiguration
    {
        GameConfig GameConfig;
        ApplicationProperties ApplicationProperties;
        ApplicationArgs ApplicationArgs;
    };
    class GameApplication : public Application
    {
    public:
        GameApplication(RuntimeConfiguration config);
        ~GameApplication() override;

    private:
        void LoadLocalizationFiles();
        void InitializeScripting();
        void LoadAssetRegistry();
        void LoadStartingScene();

    private:
        RuntimeConfiguration m_Config;
        EditorAssetManager m_AssetManager;
        Locale::Domain m_LocaleDomain;
        Ref<Scene> m_ActiveScene;
        Ref<FrameBuffer> m_FrameBuffer;
        Ref<BeeEngine::Runtime::GameLayer> m_GameLayer;
    };
} // namespace BeeEngine::Runtime