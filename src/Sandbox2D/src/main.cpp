//#include "TestLayer.h"
#include "WebGPUTestLayer.h"
//#include "BeeEngine.h"

class Game: public BeeEngine::Application
{
public:
    Game(const BeeEngine::WindowProperties& properties)
    : BeeEngine::Application(properties)
    {
        BeeEngine::Ref<BeeEngine::Layer> layer;
        if(properties.PreferredRenderAPI == Vulkan)
            //layer = BeeEngine::CreateRef<TestLayer>();
            return;
        else if(properties.PreferredRenderAPI == WebGPU)
            layer = BeeEngine::CreateRef<WebGPUTestLayer>();
        else
            BeeError("Renderer API is not chosen");
        PushLayer(layer);
    }

    virtual ~Game() override
    {

    }
};

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication()
{
    constexpr static WindowProperties properties = {1280, 720, "Pochemu", VSync::Off, RenderAPI::WebGPU};
    return new Game(properties);
}
