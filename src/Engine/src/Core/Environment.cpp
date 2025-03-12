#include "Environment.h"
#include <cstdlib>
#include "FileSystem/File.h"
#include "Logging/Log.h"

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
        CacheDirectory = CreatePath(Environment::GetEnvVariable("XDG_CACHE_HOME").value_or(HomeDirectory() / ".cache"), name);
        TempDirectory = CreatePath("/tmp", name);
      ConfigDirectory = CreatePath(Environment::GetEnvVariable("XDG_CONFIG_HOME").value_or(HomeDirectory() / ".config"), name);
#endif
    }
    std::optional<String> Environment::GetEnvVariable(const String& name)
    {
      //TODO: make this code thread/job safe and add caching
        const char* result = std::getenv(name.c_str());
        if (result == nullptr)
        {
          BeeCoreTrace("{} environmental variable is not set", name);
          return std::nullopt;
        }
       BeeCoreTrace("{} environmental variable is {}", name, result); 
      return String{result};
    }
}    
