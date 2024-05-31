//
// Created by Александр Лебедев on 01.07.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "WebGPUShaderModule.h"
#include "Core/DeletionQueue.h"
#include "WebGPUGraphicsDevice.h"
#include "WebGPUInstancedBuffer.h"

namespace BeeEngine::Internal
{
    WebGPUShaderModule::WebGPUShaderModule(in<std::string> wgsl, ShaderType type, in<BufferLayout> layout)
        : m_Type(type)
    {
        InitResources(layout);
        WGPUShaderModuleWGSLDescriptor wgslDesc{};
        wgslDesc.code = wgsl.c_str();
        wgslDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        wgslDesc.chain.next = nullptr;

        /*WGPUShaderModuleSPIRVDescriptor spirvDesc{};
        spirvDesc.code = spirvCode.data();
        spirvDesc.codeSize = spirvCode.size();
        spirvDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        spirvDesc.chain.next = nullptr;*/

        WGPUShaderModuleDescriptor shaderDesc{};
        shaderDesc.nextInChain = &wgslDesc.chain;
        shaderDesc.label = "Shader Module";

        m_ShaderModule = wgpuDeviceCreateShaderModule(WebGPUGraphicsDevice::GetInstance().GetDevice(), &shaderDesc);
    }
    WebGPUShaderModule::~WebGPUShaderModule()
    {
        DeletionQueue::Frame().PushFunction(
            [shaderModule = m_ShaderModule, layouts = m_BindGroupLayouts]()
            {
                for (auto [_, bindGroupLayout] : layouts)
                {
                    wgpuBindGroupLayoutRelease(
                        bindGroupLayout); // TODO: check why it fails if released in DeletionQueue::Main()
                }
                wgpuShaderModuleRelease(shaderModule);
            });
    }
    constexpr static WGPUVertexFormat ShaderDataTypeToWGPIU(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
                return WGPUVertexFormat_Float32;
            case ShaderDataType::Float2:
                return WGPUVertexFormat_Float32x2;
            case ShaderDataType::Float3:
                return WGPUVertexFormat_Float32x3;
            case ShaderDataType::Float4:
                return WGPUVertexFormat_Float32x4;
            case ShaderDataType::UInt:
                return WGPUVertexFormat_Uint32;
            case ShaderDataType::UInt2:
                return WGPUVertexFormat_Uint32x2;
            case ShaderDataType::UInt3:
                return WGPUVertexFormat_Uint32x3;
            case ShaderDataType::UInt4:
                return WGPUVertexFormat_Uint32x4;
            case ShaderDataType::Int:
                return WGPUVertexFormat_Sint32;
            case ShaderDataType::Int2:
                return WGPUVertexFormat_Sint32x2;
            case ShaderDataType::Int3:
                return WGPUVertexFormat_Sint32x3;
            case ShaderDataType::Int4:
                return WGPUVertexFormat_Sint32x4;
            case ShaderDataType::Bool:
            case ShaderDataType::Half:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            case ShaderDataType::NoneData:
                BeeCoreError("Unknown ShaderDataType");
                break;
        }
        return WGPUVertexFormat_Undefined;
    }

    void WebGPUShaderModule::InitResources(in<BufferLayout> layout)
    {
        if (m_Type == ShaderType::Vertex)
        {
            m_PointBufferLayout.arrayStride = layout.GetStride();
            m_PointBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
            auto& inElements = layout.GetInputElements();
            for (auto& element : inElements)
            {
                WGPUVertexAttribute attribute{};
                attribute.format = ShaderDataTypeToWGPIU(element.GetType());
                attribute.offset = element.GetOffset();
                attribute.shaderLocation = element.GetLocation();
                m_PointAttributes.push_back(attribute);
            }
            m_PointBufferLayout.attributeCount = m_PointAttributes.size();
            m_PointBufferLayout.attributes = m_PointAttributes.data();

            // Instance buffer
            m_InstanceBufferLayout.arrayStride = layout.GetInstancedStride();
            if (m_InstanceBufferLayout.arrayStride == 0)
            {
                memset(&m_InstanceBufferLayout, 0, sizeof(WGPUVertexBufferLayout));
                return;
            }
            m_InstanceBufferLayout.stepMode = WGPUVertexStepMode_Instance;
            auto& instancedElements = layout.GetInstancedElements();
            for (auto& element : instancedElements)
            {
                WGPUVertexAttribute attribute{};
                attribute.format = ShaderDataTypeToWGPIU(element.GetType());
                attribute.offset = element.GetOffset();
                attribute.shaderLocation = element.GetLocation();
                m_InstanceAttributes.push_back(attribute);
            }
            m_InstanceBufferLayout.attributeCount = m_InstanceAttributes.size();
            m_InstanceBufferLayout.attributes = m_InstanceAttributes.data();
        }
        auto& device = WebGPUGraphicsDevice::GetInstance();
        auto& uniformElements = layout.GetUniformElements();
        if (uniformElements.empty())
            return;
        uint32_t index = uniformElements[0].GetBindingSet();
        uint32_t currentSet = 0;
        m_BindGroupLayouts.emplace_back();
        for (auto& element : uniformElements)
        {
            if (element.GetBindingSet() != index)
            {
                m_BindGroupLayouts.emplace_back();
                WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
                bindGroupLayoutDesc.nextInChain = nullptr;
                bindGroupLayoutDesc.label = "BindGroupLayout";
                bindGroupLayoutDesc.entryCount = (uint32_t)m_BindGroupLayoutEntries.size();
                bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();
                m_BindGroupLayouts[currentSet] =
                    std::make_pair(index, wgpuDeviceCreateBindGroupLayout(device.GetDevice(), &bindGroupLayoutDesc));
                m_BindGroupLayoutEntries.clear();
                index = element.GetBindingSet();
                currentSet++;
            }
            WGPUBindGroupLayoutEntry entry{};
            device.SetDefault(entry);
            entry.binding = element.GetLocation();
            entry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
            switch (element.GetType())
            {
                case ShaderUniformDataType::Data:
                    entry.buffer.type = WGPUBufferBindingType_Uniform;
                    entry.buffer.hasDynamicOffset = false;
                    entry.buffer.minBindingSize = element.GetSize();
                    break;
                case ShaderUniformDataType::SampledTexture:
                    entry.texture.sampleType = WGPUTextureSampleType_Float;
                    entry.texture.viewDimension = WGPUTextureViewDimension_2D;
                    entry.texture.multisampled = false;
                    break;
                case ShaderUniformDataType::Sampler:
                    entry.sampler.type = WGPUSamplerBindingType_Filtering;
                    break;
                case ShaderUniformDataType::Unknown:
                    BeeCoreError("Unknown ShaderUniformDataType");
                    break;
            }
            m_BindGroupLayoutEntries.push_back(entry);
        }
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.nextInChain = nullptr;
        bindGroupLayoutDesc.label = "BindGroupLayout";
        bindGroupLayoutDesc.entryCount = (uint32_t)m_BindGroupLayoutEntries.size();
        bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();
        m_BindGroupLayouts[currentSet] =
            std::make_pair(index, wgpuDeviceCreateBindGroupLayout(device.GetDevice(), &bindGroupLayoutDesc));
    }

    Scope<InstancedBuffer> WebGPUShaderModule::CreateInstancedBuffer()
    {
        static constexpr size_t MAX_INSTANCED_BUFFER_COUNT = 10000;
        if (m_Type != ShaderType::Vertex)
        {
            BeeCoreError("Instanced buffer can be created only for vertex shader");
            return nullptr;
        }
        if (m_InstanceBufferLayout.arrayStride == 0)
        {
            BeeCoreTrace("Instanced buffer layout is empty");
            return nullptr;
        }
        BeeCoreTrace("Creating instanced buffer");
        return CreateScope<WebGPUInstancedBuffer>(m_InstanceBufferLayout, MAX_INSTANCED_BUFFER_COUNT);
    }
} // namespace BeeEngine::Internal
#endif