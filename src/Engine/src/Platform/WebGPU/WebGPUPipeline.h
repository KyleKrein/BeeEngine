//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once

#include "Renderer/Pipeline.h"
#include "WebGPUShaderModule.h"
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUPipeline: public Pipeline
    {
    public:
        ~WebGPUPipeline() override;
        WebGPUPipeline(const Ref<ShaderModule>&vertexShader, const Ref<ShaderModule>& fragmentShader,BindingSet* bindingSet);
        void Bind(void* commandBuffer) override;
    private:
        WGPURenderPipeline m_Pipeline;
        WGPUPipelineLayout m_PipelineLayout;
        std::unordered_map<ShaderType, Ref<ShaderModule>> m_ShaderModules;
        bool m_IsRender;
    };
}
