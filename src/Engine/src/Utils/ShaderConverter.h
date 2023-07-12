#pragma once

#include "Core/TypeDefines.h"
#include <vector>
#include "glslang/Include/ResourceLimits.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/ShaderTypes.h"

namespace BeeEngine
{
    enum class ShaderStage
    {
        Vertex,
        TessControl,
        TessEvaluation,
        Geometry,
        Fragment,
        Compute,
        ShaderStageMax,
    };
    class ShaderConverter
    {
    public:
        static bool AnalyzeGLSL(const ShaderType &stage, out<BufferLayoutBuilder> layout, out<std::string> glsl);
        static bool GLSLtoSPV(const ShaderStage shader_type, const char *pshader,
                                  std::vector<uint32_t> &spirv);
        static bool SPVtoWGSL(const std::vector<uint32_t> &spirv, std::string &wgsl);
        static BufferLayout GenerateLayout(in<std::vector<uint32_t>>spirv, BufferLayoutBuilder& layout);
        static BufferLayout GenerateLayout(in<std::string>wgsl, in<std::string> path, BufferLayoutBuilder& layout);
        static void Init();
        static void Finalize();
    private:
        static void InitResources(TBuiltInResource &Resources);
    };
}