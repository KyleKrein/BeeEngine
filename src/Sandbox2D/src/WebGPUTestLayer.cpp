//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUTestLayer.h"
#include "Renderer/Vertex.h"
#include "ext/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Platform/WebGPU/WebGPUTexture2D.h"

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

    m_InstanceBuffer.resize(135);
    for (auto& buffer:m_InstanceBuffer)
    {
        buffer.resize(135);
    }

    for (int i = 0; i < 135; ++i)
    {
        for (int j = 0; j < 135; ++j)
        {
            m_InstanceBuffer[i][j].Model = glm::translate(glm::mat4(1.0f), glm::vec3(i, j, 0.0f));
        }
    }
    BeeEngine::FrameBufferPreferences preferences;
    preferences.Width = 1280 * 2;
    preferences.Height = 720 * 2;
    preferences.Attachments = {BeeEngine::FrameBufferTextureFormat::RGBA8, BeeEngine::FrameBufferTextureFormat::Depth};
    m_FrameBuffer = BeeEngine::FrameBuffer::Create(preferences);
    m_FrameBuffer->Resize(300,300);
}

void WebGPUTestLayer::OnDetach()
{

}
static float angle = 0.0f;
static float angle2 = 0.0f;
static BeeEngine::Scope<BeeEngine::Internal::WebGPUTexture2D> framebufferTexture;
static BeeEngine::Ref<BeeEngine::BindingSet> framebufferSet;
void WebGPUTestLayer::OnUpdate()
{
    m_CameraController.OnUpdate();
    m_CameraUniformBuffer->SetData((void*)glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()), sizeof(glm::mat4));

    //InstanceBufferData first;
    //InstanceBufferData second;
    //second.Model = glm::translate(second.Model, glm::vec3(0.5f, 0.5f, 0.0f));
    //second.Model = glm::rotate(second.Model, angle2+=0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
    //second.Color = BeeEngine::Color4::Red;

    std::vector<BeeEngine::BindingSet*> bindingSets = {m_ModelBindingSet.get(), m_ForestTextureBindingSet.get()};

    m_FrameBuffer->Bind();
    for (int i = 0; i < 135; ++i)
    {
        for (int j = 0; j < 135; ++j)
        {
            BeeEngine::Renderer::SubmitInstance(*m_Model, bindingSets, {(BeeEngine::byte*)&m_InstanceBuffer[i][j], sizeof(InstanceBufferData)});
        }
    }
    //BeeEngine::Renderer::SubmitInstance(*m_Model, bindingSets, {(BeeEngine::byte*)&first, sizeof(InstanceBufferData)});
    //BeeEngine::Renderer::SubmitInstance(*m_Model, bindingSets, {(BeeEngine::byte*)&second, sizeof(InstanceBufferData)});

    //InstanceBufferData third;
    //third.Model = glm::translate(third.Model, glm::vec3(-0.5f, -0.5f, 0.0f));
    //third.Color = BeeEngine::Color4::Blue;

    //bindingSets[1] = m_BlankTextureBindingSet.get();
    m_FrameBuffer->Unbind();
    framebufferTexture = BeeEngine::CreateScope<BeeEngine::Internal::WebGPUTexture2D>((WGPUTextureView)m_FrameBuffer->GetColorAttachmentRendererID(0), 300,300);
    BeeEngine::Renderer::SubmitInstance(*m_Model, bindingSets, {(BeeEngine::byte*)&m_InstanceBuffer[0][0], sizeof(InstanceBufferData)});
    framebufferSet = BeeEngine::BindingSet::Create({
                                                                    {0, *framebufferTexture}
                                                            });
    bindingSets[1] = framebufferSet.get();
    BeeEngine::Renderer::SubmitInstance(*m_Model, bindingSets, {(BeeEngine::byte*)&m_InstanceBuffer[0][1], sizeof(InstanceBufferData)});
    //BeeEngine::Renderer::SubmitInstance(*m_Model, bindingSets, {(BeeEngine::byte*)&third, sizeof(InstanceBufferData)});
}

void WebGPUTestLayer::OnGUIRendering()
{
    ImGui::Begin("image");
    ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentRendererID(0), ImVec2(300,300));
    ImGui::End();
    m_FpsCounter.Update();
    m_FpsCounter.Render();
}
