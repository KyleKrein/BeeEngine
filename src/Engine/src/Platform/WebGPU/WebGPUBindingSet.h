//
// Created by alexl on 16.07.2023.
//

#pragma once
#if defined(BEE_COMPILE_WEBGPU)
#include "Renderer/BindingSet.h"
#include "WebGPUPipeline.h"

namespace BeeEngine::Internal
{
    class WebGPUBindingSet final : public BindingSet
    {
    public:
        WebGPUBindingSet(std::initializer_list<BindingSetElement> elements);
        void Bind(void* cmd, uint32_t index) const override;
        ~WebGPUBindingSet() override;

    private:
        mutable WGPUBindGroup m_BindGroup = nullptr;
        mutable const WebGPUPipeline* m_Pipeline = nullptr;
        mutable uint32_t m_Index = 0;
    };
} // namespace BeeEngine::Internal
#endif