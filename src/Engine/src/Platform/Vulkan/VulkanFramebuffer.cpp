//
// Created by alexl on 10.06.2023.
//

#include "VulkanFramebuffer.h"
#include "VulkanSwapChain.h"

namespace BeeEngine::Internal
{

    VulkanFramebuffer::VulkanFramebuffer(vk::Device &device, vk::RenderPass &renderPass, vk::Extent2D &swapChainExtent,
                                         vk::ImageView &imageView)
                                         : m_Device(device), m_IsInitialized(true)
    {
        vk::ImageView attachments[] = {
               imageView
        };

        vk::FramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;
        try
        {
            m_Framebuffer = device.createFramebuffer(framebufferInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        if(m_IsInitialized)
            m_Device.destroyFramebuffer(m_Framebuffer);
    }

    void
    VulkanFramebuffer::CreateFramebuffers(vk::Device &device, vk::RenderPass &renderPass, vk::Extent2D &swapChainExtent,
                                          std::vector<SwapChainFrame> &framesToGetFramebuffers)
    {
        for (auto& frame : framesToGetFramebuffers)
        {
            vk::ImageView attachments[] = {
                    frame.ImageView
            };

            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;
            try
            {
                frame.Framebuffer = {device, device.createFramebuffer(framebufferInfo)};
            }
            catch (vk::SystemError& e)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer &&other) noexcept
    {
        m_Device = other.m_Device;
        m_Framebuffer = other.m_Framebuffer;
        m_IsInitialized = other.m_IsInitialized;

        other.m_IsInitialized = false;
    }

    VulkanFramebuffer &VulkanFramebuffer::operator=(VulkanFramebuffer &&other) noexcept
    {
        m_Device = other.m_Device;
        m_Framebuffer = other.m_Framebuffer;
        m_IsInitialized = other.m_IsInitialized;

        other.m_IsInitialized = false;
        return *this;
    }
}
