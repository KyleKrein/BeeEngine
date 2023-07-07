//
// Created by alexl on 09.06.2023.
//

#include "VulkanRenderPass.h"
#include "VulkanGraphicsDevice.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine::Internal
{

    VulkanRenderPass::VulkanRenderPass(vk::Device& device, vk::Format swapchainImageFormat)
    : m_Device(device), m_IsInitialized(true)
    {
        //Define a general attachment, with its load/store operations
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.flags = vk::AttachmentDescriptionFlags();
        colorAttachment.format = swapchainImageFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        //Declare that attachment to be color buffer 0 of the framebuffer
        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        //Renderpasses are broken down into subpasses, there's always at least one.
        vk::SubpassDescription subpass = {};
        subpass.flags = vk::SubpassDescriptionFlags();
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        //Now create the renderpass
        vk::RenderPassCreateInfo renderpassInfo = {};
        renderpassInfo.flags = vk::RenderPassCreateFlags();
        renderpassInfo.attachmentCount = 1;
        renderpassInfo.pAttachments = &colorAttachment;
        renderpassInfo.subpassCount = 1;
        renderpassInfo.pSubpasses = &subpass;

        try
        {
            m_RenderPass = m_Device.createRenderPass(renderpassInfo);
        }
        catch (vk::SystemError& e)
        {
            BeeCoreError("Failed to create render pass: {0}", e.what());
        }
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        if(m_IsInitialized)
            m_Device.destroyRenderPass(m_RenderPass);
    }

    VulkanRenderPass::VulkanRenderPass(VulkanRenderPass &&other)
    {
        m_Device = other.m_Device;
        m_RenderPass = other.m_RenderPass;
        m_IsInitialized = other.m_IsInitialized;
        other.m_IsInitialized = false;
    }

    VulkanRenderPass &VulkanRenderPass::operator=(VulkanRenderPass &&other)
    {
        m_Device = other.m_Device;
        m_RenderPass = other.m_RenderPass;
        m_IsInitialized = other.m_IsInitialized;
        other.m_IsInitialized = false;
        return *this;
    }
}
