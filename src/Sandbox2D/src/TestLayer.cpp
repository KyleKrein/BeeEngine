//
// Created by Александр Лебедев on 06.05.2023.
//

#include <sstream>
#include "TestLayer.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
TestLayer::TestLayer()
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
    CreateCommandBuffers();
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
    currentTime = glfwGetTime();
    double delta = currentTime - lastTime;

    if (delta >= 1) {
        int framerate{ std::max(1, int(numFrames / delta)) };
        std::stringstream title;
        title << "Running at " << framerate << " fps.";
        glfwSetWindowTitle((GLFWwindow*)BeeEngine::WindowHandler::GetInstance()->GetWindow(), title.str().c_str());
        lastTime = currentTime;
        numFrames = -1;
        framerate = float(1000.0 / framerate);
    }

    ++numFrames;

    DrawFrame();
    //BeeEngine::Renderer2D::EndScene();
    //m_FpsCounter.Update();
}

void TestLayer::OnGUIRendering()
{
    //m_FpsCounter.Render();
}
static bool ResizeEvent(BeeEngine::WindowResizeEvent& event)
{
    BeeInfo("Window resized to {0}x{1}", event.GetWidth(), event.GetHeight());
    return true;
}
void TestLayer::OnEvent(BeeEngine::EventDispatcher &e)
{
    e.Dispatch<BeeEngine::WindowResizeEvent&, BeeEngine::EventType::WindowResize>(ResizeEvent);
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
    auto pipelineConfig = BeeEngine::Internal::VulkanPipeline::DefaultPipelineConfigInfo(device.GetSwapChain().GetExtent().width, device.GetSwapChain().GetExtent().height);
    pipelineConfig.renderPass = device.GetSwapChain().GetRenderPass();
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_Pipeline = BeeEngine::CreateScope<BeeEngine::Internal::VulkanPipeline>((*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice()).GetDevice(),
                                                                             "Shaders/VulkanTestShader.vert","Shaders/VulkanTestShader.frag", pipelineConfig);
}

void TestLayer::CreateCommandBuffers()
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
    m_CommandBuffers.resize(device.GetSwapChain().ImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.GetCommandPool().GetHandle();
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    if(vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
    {
        BeeError("Failed to allocate command buffers");
    }
}

void TestLayer::DrawFrame()
{
    uint32_t imageIndex;
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
    auto result = device.GetSwapChain().AcquireNextImage(&imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        BeeInfo("Recreating swap chain");
        int width = 0, height = 0;
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize((GLFWwindow*)BeeEngine::WindowHandler::GetInstance()->GetWindow(), &width, &height);
            glfwWaitEvents();
        }
        device.WindowResized(width, height);
        CreatePipeline();
        return;
    }
    RecordCommandBuffers(imageIndex);
    result = device.GetSwapChain().SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        BeeInfo("Recreating swap chain");
        int width = 0, height = 0;
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize((GLFWwindow*)BeeEngine::WindowHandler::GetInstance()->GetWindow(), &width, &height);
            glfwWaitEvents();
        }
        device.WindowResized(width, height);
        CreatePipeline();
        return;
    }
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

void TestLayer::RecordCommandBuffers(uint32_t imageIndex)
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
    {
        BeeError("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = device.GetSwapChain().GetRenderPass();
    renderPassInfo.framebuffer = device.GetSwapChain().GetFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = device.GetSwapChain().GetExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkQueueWaitIdle(device.GetGraphicsQueue().GetQueue());
    vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    m_Pipeline->Bind(m_CommandBuffers[imageIndex]);
    m_Model->Bind(m_CommandBuffers[imageIndex]);
    m_Model->Draw(m_CommandBuffers[imageIndex]);

    vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

    if(vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
    {
        BeeError("Failed to record command buffer");
    }
}


