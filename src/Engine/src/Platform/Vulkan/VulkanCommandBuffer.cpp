//
// Created by alexl on 10.06.2023.
//

#include "VulkanCommandBuffer.h"
#include "Core/Logging/Log.h"
#include "Core/Color4.h"


namespace BeeEngine::Internal
{

    void VulkanCommandBuffer::RecordDrawCommands(VulkanRenderPass& renderPass, vk::Extent2D& swapChainExtent, vk::Pipeline& pipeline)
    {
        vk::CommandBufferBeginInfo beginInfo;
        try
        {
            m_CommandBuffer.begin(beginInfo);
        }
        catch (vk::SystemError& e)
        {
            BeeCoreError("Failed to begin recording command buffer!");
        }

        vk::RenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.renderPass = renderPass.GetHandle();
        renderPassInfo.framebuffer = m_Framebuffer;
        renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;
        vk::ClearValue clearColor = vk::ClearColorValue(std::array<float, 4>{Color4::CornflowerBlue.R, Color4::CornflowerBlue.G, Color4::CornflowerBlue.B, Color4::CornflowerBlue.A});
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        m_CommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        m_CommandBuffer.draw(3, 1, 0, 0);
        m_CommandBuffer.endRenderPass();

        try
        {
            m_CommandBuffer.end();
        }
        catch (vk::SystemError& e)
        {
            BeeCoreError("Failed to finish recording command buffer!");
        }
    }
}
