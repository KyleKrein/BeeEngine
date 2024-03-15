//
// Created by Aleksandr on 22.02.2024.
//

#include "VulkanRendererAPI.h"

#include "Utils.h"
#include "Core/Application.h"
#include "Renderer/CommandBuffer.h"

namespace BeeEngine::Internal
{
    VulkanRendererAPI::VulkanRendererAPI()
    {
    }

    VulkanRendererAPI::~VulkanRendererAPI()
    {
    }

    void VulkanRendererAPI::Init()
    {
        m_GraphicsDevice = &VulkanGraphicsDevice::GetInstance();
        m_Window = WindowHandler::GetInstance();
        m_Device = m_GraphicsDevice->GetDevice();
        CreateCommandBuffers();
        DeletionQueue::Main().PushFunction([this]
        {
            FreeCommandBuffers();
        });
    }

    CommandBuffer VulkanRendererAPI::BeginFrame()
    {
        BeginFrame:
        if (m_GraphicsDevice->SwapChainRequiresRebuild())
        {
            BeeCoreTrace("Rebuilding swapchain");
            RecreateSwapChain();
        }
        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        auto result = swapchain.AcquireNextImage(&m_CurrentImageIndex);
        if(result == vk::Result::eErrorOutOfDateKHR)
        {
            m_GraphicsDevice->RequestSwapChainRebuild();
            goto BeginFrame;
        }
        else if(result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            BeeCoreError("Failed to acquire next image");
        }
        auto cmd = GetCurrentCommandBuffer().GetHandleAs<vk::CommandBuffer>();
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        if(cmd.begin(&beginInfo) != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to begin recording command buffer");
        }
        return {cmd};
    }

    void VulkanRendererAPI::StartMainRenderPass(CommandBuffer commandBuffer)
    {
        BeeExpects(commandBuffer == GetCurrentCommandBuffer());

        auto& swapchain = m_GraphicsDevice->GetSwapChain();

        m_GraphicsDevice->TransitionImageLayout(swapchain.GetImage(m_CurrentImageIndex), swapchain.GetFormat(),
            vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
        vk::RenderingInfo renderingInfo{};

        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.imageView = swapchain.GetImageView(m_CurrentImageIndex);
        colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.clearValue = vk::ClearColorValue{0.1f, 0.1f, 0.1f, 1.0f};

        renderingInfo.renderArea = vk::Rect2D{{0, 0}, swapchain.GetExtent()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        //m_GraphicsDevice->GetGraphicsQueue().waitIdle();
        auto cmd = commandBuffer.GetHandleAs<vk::CommandBuffer>();
        cmd.beginRendering(&renderingInfo, g_vkDynamicLoader);
        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain.GetExtent().width;
        viewport.height = (float)swapchain.GetExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{0,0};
        scissor.extent = swapchain.GetExtent();
        cmd.setViewport(0, 1, &viewport);
        cmd.setScissor(0, 1, &scissor);
    }

    void VulkanRendererAPI::EndMainRenderPass(CommandBuffer commandBuffer)
    {
        BeeExpects(commandBuffer == GetCurrentCommandBuffer());
        auto cmd = commandBuffer.GetHandleAs<vk::CommandBuffer>();
        cmd.endRendering(g_vkDynamicLoader);
    }

    void VulkanRendererAPI::EndFrame()
    {
        SubmitCommandBuffer(GetCurrentCommandBuffer());
        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        m_GraphicsDevice->TransitionImageLayout(swapchain.GetImage(m_CurrentImageIndex), swapchain.GetFormat(),
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
        auto result = swapchain.PresentImage(&m_CurrentImageIndex);
        if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            m_GraphicsDevice->RequestSwapChainRebuild();
        }
        else if(result != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to present swap chain image");
        }
    }

    RenderPass VulkanRendererAPI::GetMainRenderPass() const
    {
        BeeCoreWarn("Trying to get renderpass in vulkan with dynamic rendering");
        return {nullptr};
    }

    CommandBuffer VulkanRendererAPI::GetCurrentCommandBuffer() const
    {
        return {m_CommandBuffers[m_CurrentImageIndex]};
    }

    void VulkanRendererAPI::DrawInstanced(Model& model, InstancedBuffer& instancedBuffer,
        const std::vector<BindingSet*>& bindingSets, uint32_t instanceCount)
    {
        /*auto cmd = GetCurrentCommandBuffer().GetHandleAs<vk::CommandBuffer>();
        auto& pipeline = model.GetPipeline();
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetHandle());
        cmd.bindVertexBuffers(0, 1, &model.GetVertexBuffer().GetHandle(), &model.GetVertexBufferOffset());
        cmd.bindIndexBuffer(model.GetIndexBuffer().GetHandle(), model.GetIndexBufferOffset(), vk::IndexType::eUint32);
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.GetLayout(), 0, bindingSets.size(), bindingSets.data(), 0, nullptr);
        cmd.drawIndexed(model.GetIndexCount(), instanceCount, 0, 0, 0);*/
    }

    void VulkanRendererAPI::SubmitCommandBuffer(const CommandBuffer& commandBuffer)
    {
        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        auto cmd = commandBuffer.GetHandleAs<vk::CommandBuffer>();
        cmd.end();
        swapchain.SubmitCommandBuffers(&cmd, 1, &m_CurrentImageIndex);
    }

    void VulkanRendererAPI::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_GraphicsDevice->GetSwapChain().ImageCount());

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = m_GraphicsDevice->GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if(m_Device.allocateCommandBuffers(&allocInfo, m_CommandBuffers.data()) != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to allocate command buffers");
        }
    }

    void VulkanRendererAPI::FreeCommandBuffers()
    {
        m_Device.freeCommandBuffers(m_GraphicsDevice->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        m_CommandBuffers.clear();
    }

    void VulkanRendererAPI::RecreateSwapChain()
    {
        uint32_t width = m_Window->GetWidth(), height = m_Window->GetHeight();
        m_GraphicsDevice->WindowResized(width, height);

        if(m_GraphicsDevice->GetSwapChain().ImageCount() != m_CommandBuffers.size())
        {
            FreeCommandBuffers();
            CreateCommandBuffers();
        }
    }
}
