#include "TestLayer.h"
//#include "BeeEngine.h"

class Game: public BeeEngine::Application
{
public:
    Game(BeeEngine::WindowProperties& properties)
    : BeeEngine::Application(properties)
    {
        BeeEngine::Ref<TestLayer> layer = BeeEngine::CreateRef<TestLayer>();
        PushLayer(layer);
    }

    virtual ~Game() override
    {

    }
private:
    TestLayer m_TestLayer;
};

BeeEngine::Application* BeeEngine::CreateApplication()
{
    WindowProperties properties = {1280, 720, "Pochemu", VSync::On};
    return new Game(properties);
}
