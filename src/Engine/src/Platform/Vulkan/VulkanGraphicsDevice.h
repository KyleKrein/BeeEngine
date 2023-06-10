//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/GraphicsDevice.h"
#include "vulkan/vulkan.hpp"
#include "VulkanInstance.h"
#include "Renderer/QueueFamilyIndices.h"
#include "VulkanGraphicsQueue.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"

namespace BeeEngine::Internal
{
    class VulkanGraphicsDevice: public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(VulkanInstance& instance);
        ~VulkanGraphicsDevice() override;

        vk::Device &GetDevice()
        {
            return m_Device;
        }

        vk::PhysicalDevice& GetPhysicalDevice()
        {
            return m_PhysicalDevice;
        }
        QueueFamilyIndices& GetQueueFamilyIndices()
        {
            return m_QueueFamilyIndices;
        }
        VulkanGraphicsQueue& GetGraphicsQueue()
        {
            return *m_GraphicsQueue;
        }
        VulkanGraphicsQueue& GetPresentQueue()
        {
            return *m_PresentQueue;
        }
        VulkanSurface& GetSurface()
        {
            return *m_Surface;
        }
        VulkanSwapChain& GetSwapChain()
        {
            return *m_SwapChain;
        }
        VulkanPipeline& GetPipeline()
        {
            return *m_Pipeline;
        }

        VulkanCommandPool &GetCommandPool()
        {
            return *m_CommandPool;
        }

    private:


        //


        struct DeviceHandle
        {
            vk::Device device;
            ~DeviceHandle()
            {
                    device.destroy();
            }
        };
        QueueFamilyIndices m_QueueFamilyIndices;

        Ref<VulkanSurface> m_Surface;
        DeviceHandle m_DeviceHandle;
        Ref<VulkanSwapChain> m_SwapChain;
        vk::Device m_Device;
        vk::PhysicalDevice m_PhysicalDevice;
        Ref<VulkanGraphicsQueue> m_GraphicsQueue;
        Ref<VulkanGraphicsQueue> m_PresentQueue;

        Ref<VulkanPipeline> m_Pipeline;
        VulkanCommandBuffer m_MainCommandBuffer;
        Ref<VulkanCommandPool> m_CommandPool;

        void LogDeviceProperties(vk::PhysicalDevice &device) const;

        bool IsSuitableDevice(const vk::PhysicalDevice &device) const;

        bool CheckDeviceExtensionSupport(const vk::PhysicalDevice &device,
                                         const std::vector<const char *> &extensions) const;

        QueueFamilyIndices FindQueueFamilies();
        void CreatePhysicalDevice(const VulkanInstance &instance);
        void CreateLogicalDevice();
    };
}
