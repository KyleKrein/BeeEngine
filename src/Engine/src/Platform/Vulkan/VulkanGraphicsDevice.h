//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "vk_mem_alloc.h"
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

        void WindowResized(uint32_t width, uint32_t height) override;

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

        const SwapChainSupportDetails& GetSwapChainSupportDetails()
        {
            return m_SwapChainSupportDetails;
        }

        VulkanCommandPool &GetCommandPool()
        {
            return *m_CommandPool;
        }

        // Buffer Helper Functions
        void CreateBuffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &buffer,
                VkDeviceMemory &bufferMemory);

        VkCommandBuffer BeginSingleTimeCommands();

        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, vk::DeviceSize size);

        void CopyBufferToImage(
                VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void CreateImageWithInfo(
                const VkImageCreateInfo &imageInfo,
                VkMemoryPropertyFlags properties,
                VkImage &image,
                VkDeviceMemory &imageMemory);
        VkFormat FindSupportedFormat(
                const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        vk::PhysicalDeviceProperties properties;

    private:

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        //


        struct DeviceHandle
        {
            vk::Device device;
            VmaAllocator allocator;
            ~DeviceHandle()
            {
                vmaDestroyAllocator(allocator);
                device.destroy();
            }
        };
        QueueFamilyIndices m_QueueFamilyIndices;
        SwapChainSupportDetails m_SwapChainSupportDetails;

        void CreateSwapChainSupportDetails();

        Ref<VulkanSurface> m_Surface;
        DeviceHandle m_DeviceHandle;
        Scope<VulkanSwapChain> m_SwapChain;
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
        void InitializeVulkanMemoryAllocator(VulkanInstance &instance);
    };
}
