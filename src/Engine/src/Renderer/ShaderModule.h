//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include "InstancedBuffer.h"
#include "Renderer/BufferLayout.h"
#include "ShaderTypes.h"

namespace BeeEngine
{
    class ShaderModule
    {
    public:
        virtual ~ShaderModule() = default;
        [[nodiscard]] virtual ShaderType GetType() const = 0;
        [[nodiscard]] static Ref<ShaderModule> Create(const String& path, ShaderType type, bool loadFromCache = true);
        [[nodiscard]] static String GetCachePath() { return s_CachePath; }
        static void SetCachePath(const String& path) { s_CachePath = path; }
        [[nodiscard]] virtual Scope<InstancedBuffer> CreateInstancedBuffer() = 0;

    private:
        [[nodiscard]] static std::vector<uint32_t> CompileGLSLToSpirV(const String& path,
                                                                      ShaderType type,
                                                                      std::string& glsl,
                                                                      BufferLayoutBuilder& builder,
                                                                      out<BufferLayout> layout);
        [[nodiscard]] static std::vector<uint32_t> LoadSpirVFromCache(const String& path);
        [[nodiscard]] static std::vector<char> ReadGLSLShader(const String& path);
        [[nodiscard]] static std::string CompileSpirVToWGSL(in<std::vector<uint32_t>> spirvCode,
                                                            in<std::string> newPath);
        [[nodiscard]] static std::string LoadWGSLFromCache(const String& path);
        [[nodiscard]] static BufferLayout LoadBufferLayoutFromCache(const String& path);
        [[nodiscard]] static std::string
        LoadWGSL(const String& path, ShaderType type, bool loadFromCache, out<BufferLayout> layout);
        static bool LoadSpirV(const String& path,
                              ShaderType type,
                              bool loadFromCache,
                              out<std::vector<uint32_t>> spirv,
                              out<BufferLayout> layout);
        static constexpr auto GetExtension(ShaderType type)
        {
            switch (type)
            {
                case ShaderType::Vertex:
                    return ".vert";
                case ShaderType::Fragment:
                    return ".frag";
                case ShaderType::Compute:
                    return ".comp";
            }
            return "";
        }
        static String s_CachePath;
    };
} // namespace BeeEngine