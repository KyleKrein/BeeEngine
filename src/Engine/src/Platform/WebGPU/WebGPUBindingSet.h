//
// Created by alexl on 16.07.2023.
//

#pragma once
#include "Renderer/BindingSet.h"
namespace BeeEngine::Internal
{
    class WebGPUBindingSet final: public BindingSet
    {
    public:
        WebGPUBindingSet(std::initializer_list<BindingSetElement> elements);
        void Bind(void* cmd) override;

        WGPUBindGroupLayout GetBindGroupLayout() const
        {
            return m_BindGroupLayout;
        }
        ~WebGPUBindingSet() override;
    private:
        WGPUBindGroupLayout m_BindGroupLayout;
        WGPUBindGroup m_BindGroup;
    };
}