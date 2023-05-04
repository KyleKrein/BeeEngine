#include "BeeEngine.h"
#include "iostream"

class Game: public BeeEngine::Application
{
public:
    Game(BeeEngine::WindowProperties& properties)
    : BeeEngine::Application(properties)
    {

    }

    virtual ~Game() override
    {

    }
private:
};

BeeEngine::Application* BeeEngine::CreateApplication()
{
    WindowProperties properties = {1280, 720, "Pochemu", VSync::On};
    return new Game(properties);
}
