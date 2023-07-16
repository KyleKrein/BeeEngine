//
// Created by alexl on 16.07.2023.
//

#pragma once
#include "Renderer/BindingSet.h"
#include "WebGPUPipeline.h"

namespace BeeEngine::Internal
{
    class WebGPUBindingSet final: public BindingSet
    {
    public:
        WebGPUBindingSet(std::initializer_list<BindingSetElement> elements);
        void Bind(void* cmd, uint32_t index) override;
        ~WebGPUBindingSet() override;
    private:
        WGPUBindGroup m_BindGroup = nullptr;
        const WebGPUPipeline* m_Pipeline = nullptr;
        uint32_t m_Index = 0;
    };
}