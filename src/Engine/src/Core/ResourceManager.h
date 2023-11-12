//
// Created by alexl on 14.05.2023.
//

#pragma once

#include <gsl/gsl>
#include "TypeDefines.h"
#include "algorithm"
#include "Debug/Instrumentor.h"
#include "Core/Application.h"
#include <sstream>
#include "Core/AssetManagement/Asset.h"
#include "Path.h"
#include <string_view>

namespace BeeEngine
{
    class ResourceManager
    {
    public:
        static std::string ProjectName;
        inline static String ProcessFilePath(std::string_view filepath)
        {
            BEE_PROFILE_FUNCTION();
            String result = String(filepath);
            std::replace(result.begin(), result.end(), '\\', '/');
            return result;
        }
        inline static String GetNameFromFilePath(std::string_view filepath)
        {
            BEE_PROFILE_FUNCTION();
            String result = ResourceManager::ProcessFilePath(filepath);
            size_t lastDot = result.find_last_of('.');
            size_t lastSlash = result.find_last_of('/') + 1;
            size_t count = lastDot == -1ul? result.size() - lastSlash: lastDot - lastSlash;
            return String(result.substr(lastSlash, count));
        }

        inline static std::string GetDynamicLibraryName(const std::string& name, OSPlatform os)
        {
            std::string fullName;
            if(os == OSPlatform::Windows)
            {
                fullName = "lib" + name + ".dll";
            }
            else if(os == OSPlatform::Mac)
            {
                fullName = "lib" + name + ".dylib";
            }
            else if(os == OSPlatform::Linux)
            {
                fullName = "lib" + name + ".so";
            }

            return fullName;
        }
        inline static std::string GetDynamicLibraryName(const std::string& name)
        {
            std::string fullName;
            if(Application::GetOsPlatform() == OSPlatform::Windows)
            {
                fullName = "lib" + name + ".dll";
            }
            else if(Application::GetOsPlatform() == OSPlatform::Mac)
            {
                fullName = "lib" + name + ".dylib";
            }
            else if(Application::GetOsPlatform() == OSPlatform::Linux)
            {
                fullName = "lib" + name + ".so";
            }

            return fullName;
        }
        inline static std::string GetStaticLibraryName(const std::string& name, OSPlatform os)
        {
            std::string fullName;
            if(os == OSPlatform::Windows)
            {
                fullName = "lib" + name + ".a";
            }
            else if(os == OSPlatform::Mac)
            {
                fullName = "lib" + name + ".a";
            }
            else if(os == OSPlatform::Linux)
            {
                fullName = "lib" + name + ".a";
            }

            return fullName;
        }
        inline static std::string GetStaticLibraryName(const std::string& name)
        {
            std::string fullName;
            if(Application::GetOsPlatform() == OSPlatform::Windows)
            {
                fullName = "lib" + name + ".a";
            }
            else if(Application::GetOsPlatform() == OSPlatform::Mac)
            {
                fullName = "lib" + name + ".a";
            }
            else if(Application::GetOsPlatform() == OSPlatform::Linux)
            {
                fullName = "lib" + name + ".a";
            }

            return fullName;
        }

        static std::string GetScriptTemplate(const std::string& scriptName)
        {
            std::ostringstream script;
            script << "using BeeEngine;\n";
            script << "using System;\n";
            script << "\n";
            script << "namespace " << ProjectName << "\n";
            script << "{\n";
            script << "\tpublic class " << scriptName << ": Behaviour\n";
            script << "\t{\n";
            script << "\t\tvoid OnUpdate()\n";
            script << "\t\t{\n";
            script << "\t\t\t// TODO: Add your code here\n";
            script << "\t\t}\n";
            script << "\t}\n";
            script << "}\n";
            return script.str();
        }

        static bool IsTexture2DExtension(const Path &extension)
        {
            return extension == ".png" or
                   extension == ".jpg" or
                   extension == ".jpeg" or
                   extension == ".bmp";
        }

        static bool IsSceneExtension(const Path &extension) noexcept
        {
            return extension == ".beescene";
        }

        static bool IsFontExtension(const Path &extension) noexcept
        {
            return extension == ".ttf";
        }

        static bool IsPrefabExtension(const Path &extension) noexcept
        {
            return extension == ".beeprefab";
        }

        static bool IsAssetExtension(const Path &extension) noexcept
        {
            return IsTexture2DExtension(extension) || IsFontExtension(extension) || IsPrefabExtension(extension);
        }

        static AssetType GetAssetTypeFromExtension(const Path &extension)
        {
            if(IsTexture2DExtension(extension))
            {
                return AssetType::Texture2D;
            }
            if(IsFontExtension(extension))
            {
                return AssetType::Font;
            }
            if(IsPrefabExtension(extension))
            {
                return AssetType::Prefab;
            }
            return AssetType::None;
        }

        static bool IsScriptExtension(const Path& extension)
        {
            return extension == ".cs";
        }
    };
}
