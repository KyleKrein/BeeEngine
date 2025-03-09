#include "Environment.h"
#include <cstdlib>

namespace BeeEngine
{
    static Path CreatePath(const String& first, const String& name)
    {
      return Path(first) / "BeeEngine" / name;
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
