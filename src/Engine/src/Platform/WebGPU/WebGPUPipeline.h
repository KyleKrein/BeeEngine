//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once
#if defined(BEE_COMPILE_WEBGPU)
#include "Renderer/Pipeline.h"
#include "WebGPUShaderModule.h"
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUPipeline : public Pipeline
    {
    public:
        ~WebGPUPipeline() override;
        WebGPUPipeline(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader);
        void Bind(void* commandBuffer) override;
        WGPUBindGroupLayout GetBindGroupLayout(uint32_t index) const { return m_BindGroupLayouts.at(index); }

        static const WebGPUPipeline& GetCurrentPipeline() { return *s_CurrentPipeline; }

    private:
        WGPURenderPipeline m_Pipeline;
        WGPUPipelineLayout m_PipelineLayout;
        std::unordered_map<uint32_t, WGPUBindGroupLayout> m_BindGroupLayouts;
        std::unordered_map<ShaderType, Ref<ShaderModule>> m_ShaderModules;
        bool m_IsRender;

        static WebGPUPipeline* s_CurrentPipeline;
    };
} // namespace BeeEngine::Internal
#endif