#include "Environment.h"
#include "FileSystem/File.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Logging/Log.h"
#include <Platform/MacOS/MacOSInternal.h>
#include <cstdlib>
#include <filesystem>

#ifdef WINDOWS
#include <windows.h> //GetModuleFileNameW
#elif defined(LINUX)
#include <limits.h>
#include <unistd.h> //readlink
#endif

namespace BeeEngine
{
    static Path CreatePath(const String& first, const String& name)
    {
        auto result = Path(first);
        std::error_code err;
        if (!File::Exists(result))
        {
            File::CreateDirectory(result);
        }
        result /= "BeeEngine";
        if (!File::Exists(result))
        {
            File::CreateDirectory(result);
        }
        result /= name;
        if (!File::Exists(result))
        {
            File::CreateDirectory(result);
        }
        return result;
    }
    Environment::Environment(const String& name)
    {
#if defined(LINUX)
        HomeDirectory = Environment::GetEnvVariable("HOME").value_or("~");
        CacheDirectory =
            CreatePath(Environment::GetEnvVariable("XDG_CACHE_HOME").value_or(HomeDirectory() / ".cache"), name);
        TempDirectory = CreatePath("/tmp", name);
        ConfigDirectory =
            CreatePath(Environment::GetEnvVariable("XDG_CONFIG_HOME").value_or(HomeDirectory() / ".config"), name);
#endif
    }
    std::optional<String> Environment::GetEnvVariable(const String& name)
    {
        // TODO: make this code thread/job safe and add caching
        const char* result = std::getenv(name.c_str());
        if (result == nullptr)
        {
            BeeCoreTrace("{} environmental variable is not set", name);
            return std::nullopt;
        }
        BeeCoreTrace("{} environmental variable is {}", name, result);
        return String{result};
    }
    Path Environment::GetResourcesDirectory()
    {
#if defined(MACOS)
        return BeeEngine::Internal::MacOS::GetResourcesPathForCurrentBundle();
#elif defined(WINDOWS)
        wchar_t path[MAX_PATH] = {0};
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return Path{std::filesystem::path{path}}.GetParent();
#elif defined(LINUX)
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        auto executable = Path{String(result, (count > 0) ? count : 0)};
        BeeCoreTrace("Exe: {}", executable);
        auto appName = executable.GetFileName().AsUTF8();
        if (appName.starts_with('.') && appName.ends_with("-wrapped"))
        {
            appName = appName.erase(0, 1);
            appName = appName.erase(appName.find("-wrapped"));
        }

        if (executable.GetParent().GetFileName() == "bin")
        {
            return executable.GetParent().GetParent() / "share" / appName;
        }
        return executable.GetParent();
#else
        return Path{std::filesystem::current_path()};
#endif
    }
} // namespace BeeEngine
