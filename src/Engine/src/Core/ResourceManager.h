//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "Core/Application.h"
#include "Core/AssetManagement/Asset.h"
#include "Debug/Instrumentor.h"
#include "Path.h"
#include "String.h"
#include "TypeDefines.h"
#include "algorithm"
#include <gsl/gsl>
#include <sstream>
#include <string_view>

namespace BeeEngine
{
    class ResourceManager
    {
    public:
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
            size_t count = lastDot == -1ul ? result.size() - lastSlash : lastDot - lastSlash;
            return String(result.substr(lastSlash, count));
        }

        inline static String GetDynamicLibraryName(const String& name, OSPlatform os)
        {
            String fullName;
            if (os == OSPlatform::Windows)
            {
                fullName = "lib" + name + ".dll";
            }
            else if (os == OSPlatform::Mac)
            {
                fullName = "lib" + name + ".dylib";
            }
            else if (os == OSPlatform::Linux)
            {
                fullName = "lib" + name + ".so";
            }

            return fullName;
        }
        inline static String GetDynamicLibraryName(const String& name)
        {
            if (Application::GetOsPlatform() == OSPlatform::Windows)
            {
                return "lib" + name + ".dll";
            }
            else if (Application::GetOsPlatform() == OSPlatform::Mac)
            {
                return "lib" + name + ".dylib";
            }
            else if (Application::GetOsPlatform() == OSPlatform::Linux)
            {
                return "lib" + name + ".so";
            }

            return "";
        }
        inline static std::string GetStaticLibraryName(const std::string& name, OSPlatform os)
        {
            std::string fullName;
            if (os == OSPlatform::Windows)
            {
                fullName = "lib" + name + ".a";
            }
            else if (os == OSPlatform::Mac)
            {
                fullName = "lib" + name + ".a";
            }
            else if (os == OSPlatform::Linux)
            {
                fullName = "lib" + name + ".a";
            }

            return fullName;
        }
        inline static std::string GetStaticLibraryName(const std::string& name)
        {
            std::string fullName;
            if (Application::GetOsPlatform() == OSPlatform::Windows)
            {
                fullName = "lib" + name + ".a";
            }
            else if (Application::GetOsPlatform() == OSPlatform::Mac)
            {
                fullName = "lib" + name + ".a";
            }
            else if (Application::GetOsPlatform() == OSPlatform::Linux)
            {
                fullName = "lib" + name + ".a";
            }

            return fullName;
        }

        static String GetScriptTemplate(const String& scriptName, const String& projectName)
        {
            std::ostringstream script;
            script << "using BeeEngine;\n";
            script << "using BeeEngine.Math;\n";
            script << "\n";
            script << "namespace " << projectName << ";\n";
            script << "public class " << scriptName << ": Behaviour\n";
            script << "{\n";
            script << "\tvoid OnUpdate()\n";
            script << "\t{\n";
            script << "\t\t// TODO: Add your code here\n";
            script << "\t}\n";
            script << "}\n";
            return String{script.str()};
        }

        static bool IsTexture2DExtension(const Path& extension)
        {
            auto ext = ToLowercase(std::string_view{extension.AsUTF8()});
            return ext == ".png" or ext == ".jpg" or ext == ".jpeg" or ext == ".bmp";
        }

        static bool IsSceneExtension(const Path& extension) noexcept
        {
            auto ext = ToLowercase(std::string_view{extension.AsUTF8()});
            return ext == ".beescene";
        }

        static bool IsFontExtension(const Path& extension) noexcept
        {
            auto ext = ToLowercase(std::string_view{extension.AsUTF8()});
            return ext == ".ttf";
        }

        static bool IsPrefabExtension(const Path& extension) noexcept
        {
            auto ext = ToLowercase(std::string_view{extension.AsUTF8()});
            return ext == ".beeprefab";
        }

        static bool IsMeshSourceExtension(const Path& extension) noexcept
        {
            auto ext = ToLowercase(std::string_view{extension.AsUTF8()});
            return ext == ".gltf" or ext == ".glb";
        }

        static bool IsAssetExtension(const Path& extension) noexcept
        {
            return IsTexture2DExtension(extension) || IsFontExtension(extension) || IsPrefabExtension(extension) ||
                   IsMeshSourceExtension(extension) || IsSceneExtension(extension);
        }

        static AssetType GetAssetTypeFromExtension(const Path& extension)
        {
            if (IsTexture2DExtension(extension))
            {
                return AssetType::Texture2D;
            }
            if (IsFontExtension(extension))
            {
                return AssetType::Font;
            }
            if (IsPrefabExtension(extension))
            {
                return AssetType::Prefab;
            }
            if (IsMeshSourceExtension(extension))
            {
                return AssetType::MeshSource;
            }
            if (IsSceneExtension(extension))
            {
                return AssetType::Scene;
            }
            return AssetType::None;
        }

        static bool IsScriptExtension(const Path& extension)
        {
            auto ext = ToLowercase(std::string_view{extension.AsUTF8()});
            return ext == ".cs";
        }
    };
} // namespace BeeEngine
