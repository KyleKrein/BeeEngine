#include <BeeEngine.h>
#include <Core/GameConfig.h>
#include "GameApplication.h"

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication(const ApplicationArgs& args)
{
    BeeEngine::Runtime::RuntimeConfiguration config{
        .GameConfig = BeeEngine::GameConfig::Deserialize(),
        .ApplicationProperties = {},
        .ApplicationArgs = args
    };
    config.ApplicationProperties.Title = config.GameConfig.Name.c_str();
    config.ApplicationProperties.Vsync = VSync::Off; //TODO: make it configurable
    return new BeeEngine::Runtime::GameApplication(std::move(config));
}