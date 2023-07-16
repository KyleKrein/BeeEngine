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
        [[nodiscard]] WGPUVertexBufferLayout GetPointBufferLayout() const
        {
            return m_PointBufferLayout;
        }
        [[nodiscard]] WGPUVertexBufferLayout GetInstanceBufferLayout() const
        {
            return m_InstanceBufferLayout;
        }

        std::vector<std::pair<uint32_t,WGPUBindGroupLayout>>& GetBindGroupLayouts()
        {
            return m_BindGroupLayouts;
        }

        [[nodiscard]] Scope<InstancedBuffer> CreateInstancedBuffer() override;
    private:
        void InitResources(in<BufferLayout> layout);

        ShaderType m_Type;
        WGPUShaderModule m_ShaderModule;
        WGPUVertexBufferLayout m_PointBufferLayout;
        std::vector<WGPUVertexAttribute> m_PointAttributes;
        WGPUVertexBufferLayout m_InstanceBufferLayout;
        std::vector<WGPUVertexAttribute> m_InstanceAttributes;
        std::vector<std::pair<uint32_t,WGPUBindGroupLayout>> m_BindGroupLayouts;
        std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
    };
}