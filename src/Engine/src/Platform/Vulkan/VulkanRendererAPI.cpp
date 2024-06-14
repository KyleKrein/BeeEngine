//
// Created by Aleksandr on 22.02.2024.
//

#include "VulkanRendererAPI.h"

#include "Core/Application.h"
#include "Core/Expected.h"
#include "Renderer/CommandBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "Utils.h"
#include "VulkanFrameBuffer.h"
#include "VulkanMaterial.h"

namespace BeeEngine::Internal
{
    VulkanRendererAPI::VulkanRendererAPI() {}

    VulkanRendererAPI::~VulkanRendererAPI()
    {
        FreeCommandBuffers();
    }

    void VulkanRendererAPI::Init()
    {
        m_GraphicsDevice = &VulkanGraphicsDevice::GetInstance();
        m_Window = WindowHandler::GetInstance();
        m_Device = m_GraphicsDevice->GetDevice();
        CreateCommandBuffers();
    }

    Expected<CommandBuffer, RendererAPI::Error> VulkanRendererAPI::BeginFrame()
    {
        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        auto result = swapchain.AcquireNextImage(&m_CurrentImageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            m_GraphicsDevice->RequestSwapChainRebuild();
            return Unexpected<RendererAPI::Error>{RendererAPI::Error::SwapchainOutdated};
        }
        else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            BeeCoreError("Failed to acquire next image");
        }
        auto cmd = GetCurrentCommandBuffer().GetBufferHandleAs<vk::CommandBuffer>();
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        if (cmd.begin(&beginInfo) != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to begin recording command buffer");
        }
        CommandBuffer commandBuffer{m_CommandBuffers[m_CurrentImageIndex], &m_RenderingQueue};
        return commandBuffer;
    }

    void VulkanRendererAPI::StartMainCommandBuffer(CommandBuffer& commandBuffer)
    {
        BeeExpects(commandBuffer == GetCurrentCommandBuffer());

        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        auto cmd = commandBuffer.GetBufferHandleAs<vk::CommandBuffer>();

        m_GraphicsDevice->TransitionImageLayout(cmd,
                                                swapchain.GetImage(m_CurrentImageIndex),
                                                swapchain.GetFormat(),
                                                vk::ImageLayout::eUndefined,
                                                vk::ImageLayout::eColorAttachmentOptimal);
        vk::RenderingInfo renderingInfo{};

        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.imageView = swapchain.GetImageView(m_CurrentImageIndex);
        colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.clearValue = vk::ClearColorValue{0.1f, 0.1f, 0.1f, 1.0f};

        /*vk::RenderingAttachmentInfo depthAttachment{};
        depthAttachment.imageView = swapchain.GetDepthImageView(m_CurrentImageIndex);
        depthAttachment.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        depthAttachment.clearValue = vk::ClearDepthStencilValue{1.0f, 0};
        */

        renderingInfo.renderArea = vk::Rect2D{{0, 0}, swapchain.GetExtent()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = nullptr;

        // m_GraphicsDevice->GetGraphicsQueue().waitIdle();
        cmd.beginRendering(&renderingInfo, g_vkDynamicLoader);
        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain.GetExtent().width;
        viewport.height = (float)swapchain.GetExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = swapchain.GetExtent();
        cmd.setViewport(0, 1, &viewport);
        cmd.setScissor(0, 1, &scissor);

        commandBuffer.BeginRecording();
    }

    void VulkanRendererAPI::EndMainCommandBuffer(CommandBuffer& commandBuffer)
    {
        BeeExpects(commandBuffer == GetCurrentCommandBuffer());
        commandBuffer.EndRecording();
        auto cmd = commandBuffer.GetBufferHandleAs<vk::CommandBuffer>();
        cmd.endRendering(g_vkDynamicLoader);
        commandBuffer.Invalidate();
    }

    void VulkanRendererAPI::EndFrame()
    {
        auto cmd = GetCurrentCommandBuffer().GetBufferHandleAs<vk::CommandBuffer>();
        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        m_GraphicsDevice->TransitionImageLayout(cmd,
                                                swapchain.GetImage(m_CurrentImageIndex),
                                                swapchain.GetFormat(),
                                                vk::ImageLayout::eColorAttachmentOptimal,
                                                vk::ImageLayout::ePresentSrcKHR);
        SubmitCommandBuffer(GetCurrentCommandBuffer());
        auto result = swapchain.PresentImage(&m_CurrentImageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            m_GraphicsDevice->RequestSwapChainRebuild();
        }
        else if (result != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to present swap chain image");
        }
    }

    CommandBuffer VulkanRendererAPI::GetCurrentCommandBuffer()
    {
        return CommandBuffer{m_CommandBuffers[m_CurrentImageIndex], &m_RenderingQueue};
    }

    void VulkanRendererAPI::DrawInstanced(CommandBuffer& commandBuffer,
                                          Model& model,
                                          InstancedBuffer& instancedBuffer,
                                          const std::vector<BindingSet*>& bindingSets,
                                          uint32_t instanceCount)
    {
        model.Bind(commandBuffer);
        instancedBuffer.Bind(commandBuffer);
        auto cmd = commandBuffer.GetBufferHandleAs<vk::CommandBuffer>();
        int32_t index = 0;
        for (auto& bindingSet : bindingSets)
        {
            bindingSet->Bind(commandBuffer, index++, ((VulkanMaterial&)model.GetMaterial()).GetPipeline());
        }
        if (model.IsIndexed()) [[likely]]
            cmd.drawIndexed(model.GetIndexCount(), instanceCount, 0, 0, 0);
        else
            cmd.draw(model.GetVertexCount(), instanceCount, 0, 0);
    }

    void VulkanRendererAPI::SubmitCommandBuffer(const CommandBuffer& commandBuffer)
    {
        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        auto cmd = commandBuffer.GetBufferHandleAs<vk::CommandBuffer>();
        cmd.end();
        swapchain.SubmitCommandBuffers(&cmd, 1, &m_CurrentImageIndex);
    }

    void VulkanRendererAPI::CopyFrameBufferImageToSwapchain(FrameBuffer& framebuffer, uint32_t attachmentIndex)
    {
        auto& fb = static_cast<Internal::VulkanFrameBuffer&>(framebuffer);
        auto& swapchain = m_GraphicsDevice->GetSwapChain();
        VulkanImage image = fb.GetColorAttachment(attachmentIndex);
        // vk::CommandBuffer cmd = m_GraphicsDevice->BeginSingleTimeCommands();
        CommandBuffer commandBuffer = GetCurrentCommandBuffer();
        auto cmd = commandBuffer.GetBufferHandleAs<vk::CommandBuffer>();
        cmd.endRendering(g_vkDynamicLoader);
        m_GraphicsDevice->TransitionImageLayout(cmd,
                                                image.Image,
                                                image.Format,
                                                vk::ImageLayout::eShaderReadOnlyOptimal,
                                                vk::ImageLayout::eTransferSrcOptimal);
        vk::Image swapchainImage = swapchain.GetImage(m_CurrentImageIndex);
        m_GraphicsDevice->TransitionImageLayout(cmd,
                                                swapchainImage,
                                                swapchain.GetFormat(),
                                                vk::ImageLayout::eUndefined,
                                                vk::ImageLayout::eTransferDstOptimal);
        m_GraphicsDevice->CopyImageToImage(cmd, image.Image, swapchainImage, image.Extent, swapchain.GetExtent());
        m_GraphicsDevice->TransitionImageLayout(cmd,
                                                image.Image,
                                                image.Format,
                                                vk::ImageLayout::eTransferSrcOptimal,
                                                vk::ImageLayout::eShaderReadOnlyOptimal);
        m_GraphicsDevice->TransitionImageLayout(cmd,
                                                swapchainImage,
                                                swapchain.GetFormat(),
                                                vk::ImageLayout::eTransferDstOptimal,
                                                vk::ImageLayout::eColorAttachmentOptimal);
        // m_GraphicsDevice->EndSingleTimeCommands(cmd);
        vk::RenderingInfo renderingInfo{};

        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.imageView = swapchain.GetImageView(m_CurrentImageIndex);
        colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eLoad;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.clearValue = vk::ClearColorValue{0.1f, 0.1f, 0.1f, 1.0f};

        renderingInfo.renderArea = vk::Rect2D{{0, 0}, swapchain.GetExtent()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = nullptr;

        cmd.beginRendering(&renderingInfo, g_vkDynamicLoader);
        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain.GetExtent().width;
        viewport.height = (float)swapchain.GetExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = swapchain.GetExtent();
        cmd.setViewport(0, 1, &viewport);
        cmd.setScissor(0, 1, &scissor);
    }

    void VulkanRendererAPI::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_GraphicsDevice->GetSwapChain().ImageCount());

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = m_GraphicsDevice->GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if (m_Device.allocateCommandBuffers(&allocInfo, m_CommandBuffers.data()) != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to allocate command buffers");
        }
    }

    void VulkanRendererAPI::FreeCommandBuffers()
    {
        m_Device.freeCommandBuffers(m_GraphicsDevice->GetCommandPool(),
                                    static_cast<uint32_t>(m_CommandBuffers.size()),
                                    m_CommandBuffers.data());
        m_CommandBuffers.clear();
    }

    void VulkanRendererAPI::RebuildSwapchain()
    {
        uint32_t width = m_Window->GetWidthInPixels(), height = m_Window->GetHeightInPixels();
        m_GraphicsDevice->WindowResized(width, height);

        if (m_GraphicsDevice->GetSwapChain().ImageCount() != m_CommandBuffers.size())
        {
            FreeCommandBuffers();
            CreateCommandBuffers();
        }
    }
} // namespace BeeEngine::Internal
