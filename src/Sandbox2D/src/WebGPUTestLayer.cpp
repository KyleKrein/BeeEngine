//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUTestLayer.h"
#include "Renderer/Vertex.h"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

void WebGPUTestLayer::OnAttach()
{
    m_CameraUniformBuffer = BeeEngine::UniformBuffer::Create(sizeof(glm::mat4));
    m_CameraController.Enable();

    m_ForestTexture = &m_AssetManager.LoadTexture("ForestTexture", "Assets/Textures/forest.png");
    m_ForestTextureBindingSet = BeeEngine::BindingSet::Create({
                                                                    {0, *m_ForestTexture}
                                                            });

    m_BlankTexture = BeeEngine::Texture2D::Create(1, 1);
    int data = 0xffffffff;
    m_BlankTexture->SetData({(BeeEngine::byte *)&data, 4});

    m_BlankTextureBindingSet = BeeEngine::BindingSet::Create({
                                                                    {0, *m_BlankTexture}
                                                            });

    m_ModelBindingSet = BeeEngine::BindingSet::Create({
                                                         {0, *m_CameraUniformBuffer}
                                                 });
    auto& material = m_AssetManager.LoadMaterial("StandardMaterial", "Shaders/WebGPUTestShader.vert", "Shaders/WebGPUTestShader.frag");

    m_InstancedBuffer = &material.GetInstancedBuffer();
    std::vector<BeeEngine::Vertex> vertexBuffer =
            {
                    {{-0.5f, -0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {0.0f, 0.0f}, },
                    {{0.5f, -0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {1.0f, 0.0f}, },
                    {{0.5f, 0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {1.0f, 1.0f}, },
                    {{-0.5f, 0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {0.0f, 1.0f} }
            };
    std::vector<uint32_t> indexBuffer = {2, 1, 0, 0, 3, 2};
    auto& mesh = m_AssetManager.LoadMesh("RectMesh", vertexBuffer, indexBuffer);
    m_Model = &m_AssetManager.LoadModel("Rectangle", material, mesh);
}

void WebGPUTestLayer::OnDetach()
{

}
static float angle = 0.0f;
static float angle2 = 0.0f;

struct InstanceBufferData
{
    /*alignas(alignof(glm::mat4))*/ glm::mat4 Model {1.0f};
    /*alignas(alignof(glm::mat4))*/ BeeEngine::Color4 Color {BeeEngine::Color4::White};
    /*alignas(alignof(glm::mat4))*/ float TilingFactor = 1.0f;
};
void WebGPUTestLayer::OnUpdate()
{
    m_CameraController.OnUpdate();
    InstanceBufferData first;

    InstanceBufferData second;
    second.Model = glm::translate(second.Model, glm::vec3(0.5f, 0.5f, 0.0f));
    //second.Model = glm::rotate(second.Model, angle2+=0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
    second.Color = BeeEngine::Color4::Red;
    std::vector<InstanceBufferData> models = {first, second};

    m_InstancedBuffer->SetData(models.data(), sizeof(InstanceBufferData) * models.size());
    m_CameraUniformBuffer->SetData((void*)glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()), sizeof(glm::mat4));

    std::vector<BeeEngine::BindingSet*> bindingSets = {m_ModelBindingSet.get(), m_ForestTextureBindingSet.get()};

    //BeeEngine::Renderer::DrawInstanced(*m_Model, *m_InstancedBuffer, bindingSets, models.size());


    InstanceBufferData third;
    third.Model = glm::translate(third.Model, glm::vec3(-0.5f, -0.5f, 0.0f));
    third.Color = BeeEngine::Color4::Blue;

    m_InstancedBuffer->SetData(&third, sizeof(InstanceBufferData));

    bindingSets[1] = m_BlankTextureBindingSet.get();
    BeeEngine::Renderer::DrawInstanced(*m_Model, *m_InstancedBuffer, bindingSets, 1);
}

void WebGPUTestLayer::OnGUIRendering()
{
    m_FpsCounter.Update();
    m_FpsCounter.Render();
}
