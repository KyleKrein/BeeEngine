//
// Created by Александр Лебедев on 01.07.2023.
//

#include "WebGPUPipeline.h"
#include "Renderer/Renderer.h"
#include "WebGPUGraphicsDevice.h"

namespace BeeEngine::Internal
{
    WebGPUPipeline::WebGPUPipeline(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader)
    : m_IsRender(true)
    {
        m_ShaderModules[ShaderType::Vertex] = vertexShader;
        m_ShaderModules[ShaderType::Fragment] = fragmentShader;

        WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
        renderPipelineDescriptor.nextInChain = nullptr;
        renderPipelineDescriptor.label = "Render Pipeline";
        //vertex
        renderPipelineDescriptor.vertex.nextInChain = nullptr;
        renderPipelineDescriptor.vertex.bufferCount = 0;
        renderPipelineDescriptor.vertex.buffers = nullptr;
        renderPipelineDescriptor.vertex.module = ((WebGPUShaderModule*)vertexShader.get())->GetHandle();
        renderPipelineDescriptor.vertex.entryPoint = "main";
        renderPipelineDescriptor.vertex.constantCount = 0;
        renderPipelineDescriptor.vertex.constants = nullptr;

        //Primitive pipeline state
        // Each sequence of 3 vertices is considered as a triangle
        renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;

        // We'll see later how to specify the order in which vertices should be
        // connected. When not specified, vertices are considered sequentially.
        renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;

        // The face orientation is defined by assuming that when looking
        // from the front of the face, its corner vertices are enumerated
        // in the counter-clockwise (CCW) order.
        renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace_CCW;

        // But the face orientation does not matter much because we do not
        // cull (i.e. "hide") the faces pointing away from us (which is often
        // used for optimization).
#if defined(DEBUG)
        renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;
#else
        renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_Front;
#endif

        //fragment
        WGPUFragmentState fragmentState{};
        fragmentState.nextInChain = nullptr;
        fragmentState.module = ((WebGPUShaderModule*)fragmentShader.get())->GetHandle();
        fragmentState.entryPoint = "main";
        fragmentState.constantCount = 0;
        fragmentState.constants = nullptr;

        renderPipelineDescriptor.fragment = &fragmentState;

        //depth stencil state
        WGPUDepthStencilState depthStencilState{};
        depthStencilState.nextInChain = nullptr;
        depthStencilState.format = WebGPUGraphicsDevice::GetInstance().GetSwapChain().GetDepthFormat();
        depthStencilState.depthWriteEnabled = true;
        depthStencilState.depthCompare = WGPUCompareFunction_Less;
        depthStencilState.stencilReadMask = 0;
        depthStencilState.stencilWriteMask = 0;
        depthStencilState.depthBias = 0;
        depthStencilState.depthBiasSlopeScale = 0;
        depthStencilState.depthBiasClamp = 0;
        depthStencilState.stencilFront.compare = WGPUCompareFunction_Always;
        depthStencilState.stencilFront.failOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilFront.passOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilBack.compare = WGPUCompareFunction_Always;
        depthStencilState.stencilBack.failOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation_Keep;
        depthStencilState.stencilBack.passOp = WGPUStencilOperation_Keep;

        renderPipelineDescriptor.depthStencil = &depthStencilState;

        //blending state
        WGPUBlendState blendState{};
        blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;
        blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
        blendState.alpha.dstFactor = WGPUBlendFactor_One;
        blendState.alpha.operation = WGPUBlendOperation_Add;

        WGPUColorTargetState colorTarget{};
        colorTarget.format = WebGPUGraphicsDevice::GetInstance().GetSwapChain().GetFormat();
        colorTarget.blend = &blendState;
        colorTarget.writeMask = WGPUColorWriteMask_All; // We could write to only some of the color channels.

        // We have only one target because our render pass has only one output color
        // attachment.
        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTarget;

        //multisample state
        WGPUMultisampleState multisampleState{};
        // Samples per pixel
        renderPipelineDescriptor.multisample.count = 1;
        // Default value for the mask, meaning "all bits on"
        renderPipelineDescriptor.multisample.mask = ~0u;
        // Default value as well (irrelevant for count = 1 anyways)
        renderPipelineDescriptor.multisample.alphaToCoverageEnabled = false;

        renderPipelineDescriptor.layout = nullptr;

        m_Pipeline = wgpuDeviceCreateRenderPipeline(WebGPUGraphicsDevice::GetInstance().GetDevice(), &renderPipelineDescriptor);
    }

    WebGPUPipeline::~WebGPUPipeline()
    {
        wgpuRenderPipelineRelease(m_Pipeline);
    }

    void WebGPUPipeline::Bind(void* commandBuffer)
    {
        if(m_IsRender)
            wgpuRenderPassEncoderSetPipeline((WGPURenderPassEncoder)(((RenderPass*)commandBuffer)->GetHandle()), m_Pipeline);
    }
}