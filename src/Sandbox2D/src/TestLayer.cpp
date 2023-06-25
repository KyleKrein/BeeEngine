//
// Created by Александр Лебедев on 06.05.2023.
//

#include <sstream>
#include "TestLayer.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
TestLayer::TestLayer()
: m_RendererAPI(BeeEngine::Internal::VulkanRendererAPI::GetInstance())
{
}

TestLayer::~TestLayer()
= default;

void TestLayer::OnAttach()
{
    //BeeEngine::Renderer::SetClearColor(BeeEngine::Color4::CornflowerBlue);
    //m_CameraController = BeeEngine::OrthographicCameraController();
    //m_ForestTexture = BeeEngine::Texture2D::Create("Assets/Textures/forest.png");
    LoadModels();
    CreatePipelineLayout();
    CreatePipeline();
}

void TestLayer::OnDetach()
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
    vkDestroyPipelineLayout(device.GetDevice(), m_PipelineLayout, nullptr);
}

void TestLayer::OnUpdate()
{
    //m_CameraController.OnUpdate();
    //BeeEngine::Renderer::Clear();
    //BeeEngine::Renderer2D::BeginScene(m_CameraController);

    //BeeEngine::Renderer2D::DrawRectangle(0,0,0.1,1,1, BeeEngine::Color4::Red);
    for (int i = 0; i < 24*5; ++i)
    {
        for (int j = 0; j < 30*5; ++j)
        {
            //BeeEngine::Renderer2D::DrawImage(j, i, 0.0f, 1,1, m_ForestTexture);
        }
    }
    m_FpsCounter.Update();
    //return;

    if(m_RendererAPI.IsFrameStarted())
    {
        auto commandBuffer = m_RendererAPI.GetCurrentCommandBuffer();
        //m_RendererAPI.BeginSwapchainRenderPass(commandBuffer);
        m_Pipeline->Bind(commandBuffer);
        m_Model->Bind(commandBuffer);
        m_Model->Draw(commandBuffer);
        //m_RendererAPI.EndSwapchainRenderPass(commandBuffer);
    }
    //BeeEngine::Renderer2D::EndScene();

}

void TestLayer::OnGUIRendering()
{
    m_FpsCounter.Render();
    //ImGui::ShowDemoWindow();
}
static bool ResizeEvent(BeeEngine::WindowResizeEvent& event)
{
    BeeInfo("Window resized to {0}x{1}", event.GetWidth(), event.GetHeight());
    return true;
}
void TestLayer::OnEvent(BeeEngine::EventDispatcher &e)
{
    //e.Dispatch<BeeEngine::WindowResizeEvent&, BeeEngine::EventType::WindowResize>(ResizeEvent);
    //m_CameraController.OnEvent(e);
}

void TestLayer::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout((*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice()).GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
    {
        BeeError("Failed to create pipeline layout");
    }
}

void TestLayer::CreatePipeline()
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
    BeeEngine::Internal::PipelineConfigInfo pipelineConfig{};
    BeeEngine::Internal::VulkanPipeline::DefaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = m_RendererAPI.GetSwapchainRenderPass();
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_Pipeline = BeeEngine::CreateScope<BeeEngine::Internal::VulkanPipeline>((*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice()).GetDevice(),
                                                                             "Shaders/VulkanTestShader.vert","Shaders/VulkanTestShader.frag", pipelineConfig);
}

void TestLayer::LoadModels()
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());

    std::vector<BeeEngine::Internal::VulkanModel::Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    m_Model = BeeEngine::CreateScope<BeeEngine::Internal::VulkanModel>(device, vertices);
}