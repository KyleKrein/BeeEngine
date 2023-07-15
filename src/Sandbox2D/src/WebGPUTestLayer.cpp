//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUTestLayer.h"
#include "Renderer/Vertex.h"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

void WebGPUTestLayer::OnAttach()
{
    auto& material = m_AssetManager.LoadMaterial("StandardMaterial", "Shaders/WebGPUTestShader.vert", "Shaders/WebGPUTestShader.frag");

    m_InstancedBuffer = &material.GetInstancedBuffer();

    glm::vec3 color = {BeeEngine::Color4::Green.R, BeeEngine::Color4::Green.G, BeeEngine::Color4::Green.B};
    std::vector<BeeEngine::Vertex> vertexBuffer =
            {
                    {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color},
                    {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color},
                    {{0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, color}
            };
    std::vector<uint32_t> indexBuffer = {0, 1, 2};
    auto& mesh = m_AssetManager.LoadMesh("TriangleMesh", vertexBuffer, indexBuffer);
    m_Model = &m_AssetManager.LoadModel("Triangle", material, mesh);
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

    BeeEngine::Renderer::DrawInstanced(*m_Model, *m_InstancedBuffer, models.size());
}

void WebGPUTestLayer::OnGUIRendering()
{
    m_FpsCounter.Update();
    m_FpsCounter.Render();
}
