//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once

#include "Renderer/ShaderModule.h"
#include "Renderer/BufferLayout.h"
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUShaderModule: public ShaderModule
    {
    public:
        ~WebGPUShaderModule() override;
        WebGPUShaderModule(in<std::string> wgsl, ShaderType type, in<BufferLayout> layout);
        [[nodiscard]] ShaderType GetType() const override
        {
            return m_Type;
        }
        [[nodiscard]] WGPUShaderModule GetHandle() const
        {
            return m_ShaderModule;
        }
        [[nodiscard]] WGPUVertexBufferLayout GetVertexBufferLayout() const
        {
            return m_VertexBufferLayout;
        }
    private:
        void InitResources(in<BufferLayout> layout);

        ShaderType m_Type;
        WGPUShaderModule m_ShaderModule;
        WGPUVertexBufferLayout m_VertexBufferLayout;
        std::vector<WGPUVertexAttribute> m_VertexAttributes;
    };
}