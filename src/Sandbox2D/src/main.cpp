// #include "TestLayer.h"
#include "Core/Application.h"
#include "Core/EntryPoint.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "JobSystem/JobScheduler.h"
#include "Renderer/Renderer.h"
#include "Windowing/ApplicationProperties.h"
// #include "BeeEngine.h"
#include "RmlUiLayer.hpp"
using namespace BeeEngine;
class Game : public BeeEngine::Application
{
public:
    Game(const ApplicationProperties& properties) : Application(properties) { PushLayer(CreateRef<RmlUiLayer>()); }

    void Update(FrameData& data) override {  }

    virtual ~Game() override {}
};

BeeEngine::Application* BeeEngine::CreateApplication(const BeeEngine::ApplicationArgs& args)
{
    static ApplicationProperties properties = {1280, 720, "Pochemu", VSync::On};
    return new Game(properties);
}
