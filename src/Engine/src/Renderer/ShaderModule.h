//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once

#include "ShaderTypes.h"
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class ShaderModule
    {
    public:
        virtual ~ShaderModule() = default;
        [[nodiscard]] virtual ShaderType GetType() const = 0;
        [[nodiscard]] static Ref<ShaderModule> Create(const String& path, ShaderType type, bool loadFromCache = true);
        [[nodiscard]] static String GetCachePath()
        {
            return s_CachePath;
        }
        static void SetCachePath(const String& path)
        {
            s_CachePath = path;
        }
    private:
        [[nodiscard]] static std::vector<uint32_t> CompileGLSLToSpirV(const String& path, ShaderType type, bool loadFromCache);
        [[nodiscard]] static std::vector<uint32_t> LoadSpirVFromCache(const String& path);
        [[nodiscard]] static std::vector<char> ReadGLSLShader(const String& path);
        static String s_CachePath;
    };
}