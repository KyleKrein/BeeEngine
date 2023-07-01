//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUTestLayer.h"

void WebGPUTestLayer::OnAttach()
{
    m_VertexShader = BeeEngine::ShaderModule::Create("Shaders/WebGPUTestShader.vert", BeeEngine::ShaderType::Vertex,
                                                     false);
    m_FragmentShader = BeeEngine::ShaderModule::Create("Shaders/WebGPUTestShader.frag", BeeEngine::ShaderType::Fragment,
                                                       false);
    m_Pipeline = BeeEngine::Pipeline::Create(m_VertexShader, m_FragmentShader);
}

void WebGPUTestLayer::OnDetach()
{

}

void WebGPUTestLayer::OnUpdate()
{
    auto cmd = BeeEngine::Renderer::GetMainRenderPass();
    m_Pipeline->Bind(&cmd);
    wgpuRenderPassEncoderDraw((WGPURenderPassEncoder)cmd.GetHandle(), 3, 1, 0, 0);
    //wgpuRenderPassEncoderDrawIndexed((WGPURenderPassEncoder)cmd.GetHandle(), 3, 1, 0, 0, 0);
}

void WebGPUTestLayer::OnGUIRendering()
{
    m_FpsCounter.Update();
    m_FpsCounter.Render();
}
