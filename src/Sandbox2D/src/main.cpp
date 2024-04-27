//#include "TestLayer.h"
#include "JobSystem/JobScheduler.h"
#include "Windowing/ApplicationProperties.h"
#include "Core/Application.h"
#include "Core/EntryPoint.h"
#include "Gui/ImGui/ImGuiExtension.h"
//#include "BeeEngine.h"
#include "VulkanTestLayer.h"
using namespace BeeEngine;
class Game: public BeeEngine::Application
{
public:
    Game(const ApplicationProperties& properties)
    : Application(properties)
    {
        PushLayer(CreateRef<VulkanTestLayer>());
    }

    void Update() override
    {

    }

    virtual ~Game() override
    {

    }
};

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication(const BeeEngine::ApplicationArgs& args)
{
    static ApplicationProperties properties = {1280, 720, "Pochemu", VSync::On};
    return new Game(properties);
}
