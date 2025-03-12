#pragma once
#include "Path.h"
#include "Property.h"
#include "String.h"
#include <optional>
namespace BeeEngine
{
    class Environment
    {
    public:
        Environment(const String& name);
        static std::optional<String> GetEnvVariable(const String& name);
      static Path GetResourcesDirectory(); //returns path, where all Shaders/Libs should be found
        Property<Path> HomeDirectory;
        Property<Path> CacheDirectory;
        Property<Path> TempDirectory;
        Property<Path> ConfigDirectory;
    };
} // namespace BeeEngine
