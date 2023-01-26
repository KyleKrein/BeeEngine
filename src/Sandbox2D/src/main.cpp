#include "BeeEngine.h"
#include "iostream"

class Game: public BeeEngine::Application
{
public:
    Game()
    {

    }

    virtual ~Game() override
    {

    }
private:
};

BeeEngine::Application* BeeEngine::CreateApplication()
{
    return new Game();
}
