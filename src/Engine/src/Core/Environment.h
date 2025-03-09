#pragma once
#include "String.h"
#include "Path.h"
#include "Property.h"
namespace BeeEngine
{
    class Environment
    {
    public:
        Environment(const String& name);
        static String GetEnvVariable(const String& name);
        Property<Path> CacheDirectory;
        Property<Path> TempDirectory;
        Property<Path> ConfigDirectory;        
    };
}    
