//
// Created by alexl on 16.07.2023.
//

#include "Renderer/BindingSet.h"
#include "Renderer/IBindable.h"
#include "WebGPUGraphicsDevice.h"
#include "WebGPUBindingSet.h"
#include "Renderer/RenderPass.h"


namespace BeeEngine::Internal
{
    void Internal::WebGPUBindingSet::Bind(void* cmd)
    {
        wgpuRenderPassEncoderSetBindGroup((WGPURenderPassEncoder)((RenderPass*)cmd)->GetHandle(), 0, m_BindGroup, 0, nullptr);
    }
    Internal::WebGPUBindingSet::WebGPUBindingSet(std::initializer_list<BindingSetElement> elements)
    : BindingSet(elements)
    {
        std::vector<WGPUBindGroupLayoutEntry> groupLayoutEntries;
        std::vector<WGPUBindGroupEntry> entries;
        groupLayoutEntries.reserve(m_Elements.size());
        entries.reserve(m_Elements.size());
        for (const auto& element : m_Elements)
        {
            auto LayoutEntry = element.Data.GetBindGroupLayoutEntry();

            LayoutEntry[0].binding = element.Binding;
            groupLayoutEntries.push_back(LayoutEntry[0]);
            if(LayoutEntry.size() > 1)
            {
                LayoutEntry[1].binding = element.Binding + 1;
                groupLayoutEntries.push_back(LayoutEntry[1]);
            }

            auto entry = element.Data.GetBindGroupEntry();

            entry[0].binding = element.Binding;
            entries.push_back(entry[0]);
            if(entry.size() > 1)
            {
                entry[1].binding = element.Binding + 1;
                entries.push_back(entry[1]);
            }
        }
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.nextInChain = nullptr;
        bindGroupLayoutDesc.label = "BindGroupLayout";
        bindGroupLayoutDesc.entryCount = (uint32_t)groupLayoutEntries.size();
        bindGroupLayoutDesc.entries = groupLayoutEntries.data();
        m_BindGroupLayout = wgpuDeviceCreateBindGroupLayout(Internal::WebGPUGraphicsDevice::GetInstance().GetDevice(), &bindGroupLayoutDesc);

        WGPUBindGroupDescriptor bindGroupDesc = {};
        bindGroupDesc.nextInChain = nullptr;
        bindGroupDesc.label = "BindGroup";
        bindGroupDesc.layout = m_BindGroupLayout;
        bindGroupDesc.entryCount = (uint32_t)entries.size();
        bindGroupDesc.entries = entries.data();
        m_BindGroup = wgpuDeviceCreateBindGroup(Internal::WebGPUGraphicsDevice::GetInstance().GetDevice(), &bindGroupDesc);
    }

    WebGPUBindingSet::~WebGPUBindingSet()
    {
        wgpuBindGroupLayoutRelease(m_BindGroupLayout);
        wgpuBindGroupRelease(m_BindGroup);
    }
}
