//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once

#include "Core/Path.h"
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
        [[nodiscard]] static Ref<ShaderModule> Create(const Path& path, ShaderType type, bool loadFromCache = true);
        [[nodiscard]] static Path GetCachePath() { return s_CachePath; }
        static void SetCachePath(const Path& path) { s_CachePath = path; }
        [[nodiscard]] virtual Scope<InstancedBuffer> CreateInstancedBuffer() = 0;

    private:
        [[nodiscard]] static std::vector<uint32_t> CompileGLSLToSpirV(
            const Path& path, ShaderType type, String& glsl, BufferLayoutBuilder& builder, out<BufferLayout> layout);
        [[nodiscard]] static std::vector<uint32_t> LoadSpirVFromCache(const Path& path);
        [[nodiscard]] static std::vector<char> ReadGLSLShader(const Path& path);
        [[nodiscard]] static String CompileSpirVToWGSL(in<std::vector<uint32_t>> spirvCode, in<Path> newPath);
        [[nodiscard]] static String LoadWGSLFromCache(const Path& path);
        [[nodiscard]] static BufferLayout LoadBufferLayoutFromCache(const Path& path);
        [[nodiscard]] static String
        LoadWGSL(const Path& path, ShaderType type, bool loadFromCache, out<BufferLayout> layout);
        static bool LoadSpirV(const Path& path,
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
        static Path s_CachePath;
    };
} // namespace BeeEngine