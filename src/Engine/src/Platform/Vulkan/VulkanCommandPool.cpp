//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#include "VulkanCommandPool.h"
#include "VulkanSwapChain.h"
#include "Core/Logging/Log.h"

namespace BeeEngine::Internal
{

    VulkanCommandPool::VulkanCommandPool(vk::Device &device, const QueueFamilyIndices& queueFamilyIndices)
    : m_Device(device)
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

        try
        {
            m_CommandPool = device.createCommandPool(commandPoolCreateInfo);
        }
        catch (vk::SystemError &e)
        {
            BeeCoreError("Failed to create command pool: {0}", e.what());
        }

        m_AllocateInfo.commandPool = m_CommandPool;
        m_AllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        m_AllocateInfo.commandBufferCount = 1;
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        m_Device.destroyCommandPool(m_CommandPool);
    }

    void VulkanCommandPool::CreateCommandBuffers(std::vector<SwapChainFrame> &commandBuffers)
    {
        for (auto& frame : commandBuffers)
        {
            //frame.CommandBuffer = CreateCommandBuffer(frame.Framebuffer.GetHandle());
        }
    }

    VulkanCommandBuffer VulkanCommandPool::CreateCommandBuffer(vk::Framebuffer& framebuffer)
    {
        auto commandBuffer = m_Device.allocateCommandBuffers(m_AllocateInfo)[0];
        m_CommandBuffers.push_back(commandBuffer);
        return {commandBuffer, framebuffer};
    }
}
#endif