//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUTestLayer.h"
#include "Renderer/Vertex.h"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

void WebGPUTestLayer::OnAttach()
{
    m_VertexShader = BeeEngine::ShaderModule::Create("Shaders/WebGPUTestShader.vert", BeeEngine::ShaderType::Vertex,
                                                     false);
    m_FragmentShader = BeeEngine::ShaderModule::Create("Shaders/WebGPUTestShader.frag", BeeEngine::ShaderType::Fragment,
                                                       false);
    m_InstancedBuffer = m_VertexShader->CreateInstancedBuffer();
    m_Pipeline = BeeEngine::Pipeline::Create(m_VertexShader, m_FragmentShader);

    glm::vec3 color = {BeeEngine::Color4::Green.R, BeeEngine::Color4::Green.G, BeeEngine::Color4::Green.B};
    std::vector<BeeEngine::Vertex> vertexBuffer =
            {
                    {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color},
                    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color},
                    {{0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color}
            };
    std::vector<uint32_t> indexBuffer = {0, 1, 2};
    m_Mesh = BeeEngine::Mesh::Create(vertexBuffer, indexBuffer);
}

void WebGPUTestLayer::OnDetach()
{

}
static float angle = 0.0f;
static float angle2 = 0.0f;
void WebGPUTestLayer::OnUpdate()
{
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, angle+=2/10, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, glm::vec3(0.1f, 0.1f, 0.1f));
    model2 = glm::rotate(model2, angle2+=0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
    std::array<glm::mat4, 2> models = {model, model2};

    m_InstancedBuffer->SetData(models.data(), sizeof(glm::mat4) * models.size());

    auto cmd = BeeEngine::Renderer::GetMainRenderPass();
    m_Pipeline->Bind(&cmd);
    auto renderPass = (WGPURenderPassEncoder)cmd.GetHandle();
    m_Mesh->Bind(&cmd);
    m_InstancedBuffer->Bind(&cmd);
    if(m_Mesh->IsIndexed())
        wgpuRenderPassEncoderDrawIndexed(renderPass, m_Mesh->GetVertexCount(), models.size(), 0, 0, 0);
    else
        wgpuRenderPassEncoderDraw(renderPass, m_Mesh->GetVertexCount(), models.size(), 0, 0);
    //wgpuRenderPassEncoderDrawIndexed((WGPURenderPassEncoder)cmd.GetHandle(), 3, 1, 0, 0, 0);
}

void WebGPUTestLayer::OnGUIRendering()
{
    m_FpsCounter.Update();
    m_FpsCounter.Render();
}
