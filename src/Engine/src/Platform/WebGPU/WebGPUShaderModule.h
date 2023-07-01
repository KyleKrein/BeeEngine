//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once

#include "Renderer/ShaderModule.h"
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUShaderModule: public ShaderModule
    {
    public:
        ~WebGPUShaderModule() override;
        WebGPUShaderModule(in<std::vector<uint32_t>> spirv, ShaderType type);
        [[nodiscard]] ShaderType GetType() const override
        {
            return m_Type;
        }
        [[nodiscard]] WGPUShaderModule GetHandle() const
        {
            return m_ShaderModule;
        }
    private:
        ShaderType m_Type;
        WGPUShaderModule m_ShaderModule;
    };
}