#pragma once

#include "Core/TypeDefines.h"
#include <vector>
#include "glslang/Include/ResourceLimits.h"

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
        static bool GLSLtoSPV(const ShaderStage shader_type, const char *pshader,
                                  std::vector<uint32_t> &spirv);
    private:
        static void InitResources(TBuiltInResource &Resources);
        static void Init();
        static void Finalize();
    };
}