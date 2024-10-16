#include "GameApplication.h"
#include <BeeEngine.h>
#include <Core/GameConfig.h>
#include <Core/OsPlatform.h>
#include <Platform/MacOS/MacOSInternal.h>

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication(const ApplicationArgs& args)
{
    if (BeeEngine::Application::GetOsPlatform() == OSPlatform::Mac)
    {
        auto old = std::filesystem::current_path();
        auto newDir = BeeEngine::Internal::MacOS::GetResourcesPathForCurrentBundle();
        std::filesystem::current_path(newDir.ToStdPath());
        BeeCoreTrace(
            "Changing working directory from {0} to {1}", old.string(), std::filesystem::current_path().string());
    }
    BeeEngine::Runtime::RuntimeConfiguration config{
        .GameConfig = BeeEngine::GameConfig::Deserialize(), .ApplicationProperties = {}, .ApplicationArgs = args};
    config.ApplicationProperties.Title = config.GameConfig.Name;
    config.ApplicationProperties.Vsync = VSync::Off; // TODO: make it configurable
    return new BeeEngine::Runtime::GameApplication(std::move(config));
}