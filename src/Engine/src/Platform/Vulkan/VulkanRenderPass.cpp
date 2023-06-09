//
// Created by alexl on 09.06.2023.
//

#include "VulkanRenderPass.h"
#include "VulkanGraphicsDevice.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine::Internal
{

    VulkanRenderPass::VulkanRenderPass(vk::Format swapChainImageFormat)
    : m_Device((*(VulkanGraphicsDevice*)&(WindowHandler::GetInstance()->GetGraphicsDevice())).GetDevice())
    {
        vk::AttachmentDescription colorAttachment;
        colorAttachment.flags = vk::AttachmentDescriptionFlags();
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass;
        subpass.flags = vk::SubpassDescriptionFlags();
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.inputAttachmentCount = 0;
        subpass.pColorAttachments = &colorAttachmentRef;

        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.flags = vk::RenderPassCreateFlags();
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        try
        {
            m_RenderPass = m_Device.createRenderPass(renderPassInfo);
        }
        catch (vk::SystemError& e)
        {
            BeeCoreError("Failed to create render pass: {0}", e.what());
        }
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        m_Device.destroyRenderPass(m_RenderPass);
    }
}
