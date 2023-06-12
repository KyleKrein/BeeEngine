#include "TestLayer.h"
//#include "BeeEngine.h"

class Game: public BeeEngine::Application
{
public:
    Game(const BeeEngine::WindowProperties& properties)
    : BeeEngine::Application(properties)
    {
        BeeEngine::Ref<TestLayer> layer = BeeEngine::CreateRef<TestLayer>();
        PushLayer(layer);
    }

    virtual ~Game() override
    {

    }
};

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication()
{
    constexpr static WindowProperties properties = {1280, 720, "Pochemu", VSync::Off, RenderAPI::Vulkan};
    return new Game(properties);
}
