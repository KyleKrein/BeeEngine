//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#pragma once

#include "vk_mem_alloc.h"
#include "Renderer/GraphicsDevice.h"
#include "vulkan/vulkan.hpp"
#include "VulkanInstance.h"
#include "Renderer/QueueFamilyIndices.h"
#include "VulkanSwapChain.h"
#include "VulkanBuffer.h"

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

        vk::Queue GetGraphicsQueue()
        {
            return m_GraphicsQueue;
        }

        vk::Queue GetPresentQueue()
        {
            return m_PresentQueue;
        }

        vk::SurfaceKHR GetSurface()
        {
            return m_Surface;
        }

        VulkanSwapChain& GetSwapChain()
        {
            return *m_SwapChain;
        }
        void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

        /*VulkanPipeline& GetPipeline()
        {
            return *m_Pipeline;
        }*/

        const SwapChainSupportDetails& GetSwapChainSupportDetails()
        {
            return m_SwapChainSupportDetails;
        }

        vk::CommandPool GetCommandPool()
        {
            return m_CommandPool;
        }

        void RequestSwapChainRebuild() override
        {
            BeeCoreInfo("Swap chain rebuild requested");
            m_SwapChainRebuildRequested = true;
        }

        bool SwapChainRequiresRebuild() override
        {
            return m_SwapChainRebuildRequested;
        }

        // Buffer Helper Functions

        VulkanBuffer CreateBuffer(
                vk::DeviceSize size,
                vk::BufferUsageFlags usage,
                VmaMemoryUsage memoryUsage) const;

        void CopyToBuffer(gsl::span<byte> data, VulkanBuffer& outBuffer) const;


        vk::CommandBuffer BeginSingleTimeCommands();

        void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);

        void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

        void CopyBufferToImage(
                vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount);

        void CreateImageWithInfo(
                 const vk::ImageCreateInfo& imageInfo,
                 const vk::ImageViewCreateInfo& imageViewInfo,
                 vk::MemoryPropertyFlags memoryProperties,
                 const VmaMemoryUsage& memoryUsage,
                 VulkanImage& outImage,
                 vk::ImageView& outImageView) const;

        vk::Format FindSupportedFormat(
        const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
                                               vk::FormatFeatureFlags features);

        vk::PhysicalDeviceProperties properties;

        static VulkanGraphicsDevice &GetInstance();

    private:
        void CreateCommandPool();

        static VulkanGraphicsDevice* s_Instance;

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void LoadKHRRayTracing();
        //


        struct DeviceHandle
        {
            vk::Device device;
            VmaAllocator allocator;
            ~DeviceHandle()
            {
                vmaDestroyAllocator(allocator);
                device.destroy();
                VulkanGraphicsDevice::s_Instance = nullptr;
            }
        };
        QueueFamilyIndices m_QueueFamilyIndices;
        SwapChainSupportDetails m_SwapChainSupportDetails;

        void CreateSwapChainSupportDetails();

        vk::SurfaceKHR m_Surface;
        DeviceHandle m_DeviceHandle;
        Scope<VulkanSwapChain> m_SwapChain;
        vk::Device m_Device;
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Queue m_GraphicsQueue;
        vk::Queue m_PresentQueue;

        //Ref<VulkanPipeline> m_Pipeline;
        vk::CommandBuffer m_MainCommandBuffer;
        vk::CommandPool m_CommandPool;
        vk::CommandBufferAllocateInfo m_CommandPoolAllocateInfo;

        struct DeletionQueueFlusher
        {
            ~DeletionQueueFlusher()
            {
                DeletionQueue::Main().Flush();
            }
        };

        DeletionQueueFlusher m_Deleter;

        bool m_SwapChainRebuildRequested = false;

        void LogDeviceProperties(vk::PhysicalDevice &device) const;

        bool IsSuitableDevice(const vk::PhysicalDevice &device) const;

        bool CheckDeviceExtensionSupport(const vk::PhysicalDevice &device,
                                         const std::vector<const char *> &extensions) const;

        QueueFamilyIndices FindQueueFamilies();
        void SelectPhysicalDevice(const VulkanInstance &instance);
        void CreateLogicalDevice();
        void CreateSurface(VulkanInstance &instance);
        void InitializeVulkanMemoryAllocator(VulkanInstance &instance);


        friend VmaAllocator GetVulkanAllocator();
    };

    inline VmaAllocator GetVulkanAllocator()
    {
        return VulkanGraphicsDevice::s_Instance->m_DeviceHandle.allocator;
    }
}
#endif