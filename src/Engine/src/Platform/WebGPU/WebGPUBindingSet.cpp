//
// Created by alexl on 16.07.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "WebGPUBindingSet.h"
#include "Renderer/BindingSet.h"
#include "Renderer/IBindable.h"
#include "Renderer/RenderPass.h"
#include "WebGPUGraphicsDevice.h"
#include "WebGPUPipeline.h"

namespace BeeEngine::Internal
{
    void Internal::WebGPUBindingSet::Bind(void* cmd, uint32_t index) const
    {
        auto* currentPipeline = &WebGPUPipeline::GetCurrentPipeline();
        BeeExpects(currentPipeline != nullptr);
        if (!m_BindGroup || m_Pipeline != currentPipeline || m_Index != index)
        {
            m_Index = index;
            m_Pipeline = currentPipeline;

            std::vector<WGPUBindGroupEntry> entries;
            entries.reserve(m_Elements.size());
            for (const auto& element : m_Elements)
            {
                auto entry = element.Data.GetBindGroupEntry();

                std::get<WGPUBindGroupEntry>(entry[0]).binding = element.Binding;
                entries.push_back(std::get<WGPUBindGroupEntry>(entry[0]));
                if (entry.size() > 1)
                {
                    std::get<WGPUBindGroupEntry>(entry[1]).binding = element.Binding + 1;
                    entries.push_back(std::get<WGPUBindGroupEntry>(entry[1]));
                }
            }
            WGPUBindGroupDescriptor bindGroupDesc = {};
            bindGroupDesc.nextInChain = nullptr;
            bindGroupDesc.label = "BindGroup";
            bindGroupDesc.layout = m_Pipeline->GetBindGroupLayout(index);
            bindGroupDesc.entryCount = (uint32_t)entries.size();
            bindGroupDesc.entries = entries.data();
            m_BindGroup =
                wgpuDeviceCreateBindGroup(Internal::WebGPUGraphicsDevice::GetInstance().GetDevice(), &bindGroupDesc);
        }
        wgpuRenderPassEncoderSetBindGroup(
            (WGPURenderPassEncoder)((RenderPass*)cmd)->GetHandle(), index, m_BindGroup, 0, nullptr);
    }
    Internal::WebGPUBindingSet::WebGPUBindingSet(std::initializer_list<BindingSetElement> elements)
        : BindingSet(elements)
    {
    }

    WebGPUBindingSet::~WebGPUBindingSet()
    {
        if (m_BindGroup)
            wgpuBindGroupRelease(m_BindGroup);
    }
} // namespace BeeEngine::Internal
#endif