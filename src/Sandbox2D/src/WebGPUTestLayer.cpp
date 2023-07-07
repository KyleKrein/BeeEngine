//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUTestLayer.h"
#include "Renderer/Vertex.h"

void WebGPUTestLayer::OnAttach()
{
    m_VertexShader = BeeEngine::ShaderModule::Create("Shaders/WebGPUTestShader.vert", BeeEngine::ShaderType::Vertex,
                                                     false);
    m_FragmentShader = BeeEngine::ShaderModule::Create("Shaders/WebGPUTestShader.frag", BeeEngine::ShaderType::Fragment,
                                                       false);
    m_Pipeline = BeeEngine::Pipeline::Create(m_VertexShader, m_FragmentShader);

    glm::vec3 color = {BeeEngine::Color4::Green.R, BeeEngine::Color4::Green.G, BeeEngine::Color4::Green.B};
    std::vector<BeeEngine::Vertex> vertexBuffer =
            {
                    {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color},
                    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color},
                    {{0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color}
            };
    m_Mesh = BeeEngine::Mesh::Create(vertexBuffer);
}

void WebGPUTestLayer::OnDetach()
{

}

void WebGPUTestLayer::OnUpdate()
{
    auto cmd = BeeEngine::Renderer::GetMainRenderPass();
    m_Pipeline->Bind(&cmd);
    auto renderPass = (WGPURenderPassEncoder)cmd.GetHandle();
    m_Mesh->Bind(&cmd);
    wgpuRenderPassEncoderDraw(renderPass, m_Mesh->GetVertexCount(), 1, 0, 0);
    //wgpuRenderPassEncoderDrawIndexed((WGPURenderPassEncoder)cmd.GetHandle(), 3, 1, 0, 0, 0);
}

void WebGPUTestLayer::OnGUIRendering()
{
    m_FpsCounter.Update();
    m_FpsCounter.Render();
}
