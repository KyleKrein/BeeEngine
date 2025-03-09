#include "Environment.h"
#include <cstdlib>
#include "FileSystem/File.h"

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
        CacheDirectory = CreatePath(Environment::GetEnvVariable("XDG_CACHE_HOME"), name);
        TempDirectory = CreatePath("/tmp", name);
      ConfigDirectory = CreatePath(Environment::GetEnvVariable("XDG_CONFIG_HOME"), name);
#endif
    }
    String Environment::GetEnvVariable(const String& name)
    {
      return {std::getenv(name.c_str())};
    }
}    
