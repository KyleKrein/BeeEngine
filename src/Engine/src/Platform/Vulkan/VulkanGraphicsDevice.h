//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#pragma once

#include "Renderer/GraphicsDevice.h"
#include "Renderer/QueueFamilyIndices.h"
#include "VulkanBuffer.h"
#include "VulkanInstance.h"
#include "VulkanSwapChain.h"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
  class GPU;
    class VulkanGraphicsDevice final : public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(VulkanInstance& instance);

        ~VulkanGraphicsDevice() override;

        void WindowResized(uint32_t width, uint32_t height) override;

        vk::Device& GetDevice() { return m_Device; }

        vk::PhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }

        QueueFamilyIndices& GetQueueFamilyIndices() { return m_QueueFamilyIndices; }

        vk::Queue GetGraphicsQueue() { return m_GraphicsQueue; }

        vk::Queue GetPresentQueue() { return m_PresentQueue; }

        vk::SurfaceKHR GetSurface() { return m_DeviceHandle.surface; }

        VulkanSwapChain& GetSwapChain() { return *m_SwapChain; }
        void
        TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
        void TransitionImageLayout(vk::CommandBuffer cmd,
                                   vk::Image image,
                                   vk::Format format,
                                   vk::ImageLayout oldLayout,
                                   vk::ImageLayout newLayout);

        /*VulkanPipeline& GetPipeline()
        {
            return *m_Pipeline;
        }*/

        const SwapChainSupportDetails& GetSwapChainSupportDetails() { return m_SwapChainSupportDetails; }

        vk::CommandPool GetCommandPool() { return m_CommandPool; }

        void RequestSwapChainRebuild() override
        {
            BeeCoreInfo("Swap chain rebuild requested");
            m_SwapChainRebuildRequested = true;
        }

        bool SwapChainRequiresRebuild() override { return m_SwapChainRebuildRequested; }

        // Buffer Helper Functions

        void CreateDescriptorSet(vk::DescriptorSetAllocateInfo& info, vk::DescriptorSet* outDescriptorSet) const;

        void DestroyDescriptorSet(vk::DescriptorSet descriptorSet) const;
        [[nodiscard]] VulkanBuffer
        CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage) const;
        void DestroyBuffer(VulkanBuffer& buffer) const;
        void DestroyImage(VulkanImage& image) const;
        void DestroyImageWithView(VulkanImage& image, vk::ImageView imageView) const;

        void CopyToBuffer(gsl::span<byte> data, VulkanBuffer& outBuffer) const;

        vk::CommandBuffer BeginSingleTimeCommands();

        void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);

        void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

        void
        CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount);
        void CopyImageToImage(
            vk::CommandBuffer cmd, vk::Image source, vk::Image destination, vk::Extent2D srcSize, vk::Extent2D dstSize);
        void CopyImageToImage(vk::Image source, vk::Image destination, vk::Extent2D srcSize, vk::Extent2D dstSize);

        void CreateImageWithInfo(const vk::ImageCreateInfo& imageInfo,
                                 const vk::ImageViewCreateInfo& imageViewInfo,
                                 vk::MemoryPropertyFlags memoryProperties,
                                 const VmaMemoryUsage& memoryUsage,
                                 VulkanImage& outImage,
                                 vk::ImageView& outImageView) const;

        vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates,
                                       vk::ImageTiling tiling,
                                       vk::FormatFeatureFlags features);

        bool HasRayTracingSupport() const { return m_HasRayTracingSupport; }

        vk::Viewport CreateVKViewport(uint32_t width, uint32_t height, float depthMin, float depthMax);

        vk::PhysicalDeviceProperties properties;

        static VulkanGraphicsDevice& GetInstance();

        uint64_t GetVRAM() const override { return m_VRAM; }

    private:
        vk::DescriptorPool m_DescriptorPool;
        mutable bool m_HasRayTracingSupport = false;

        void CreateCommandPool();

        void CreateDescriptorPool();

        static VulkanGraphicsDevice* s_Instance;

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void LoadKHRRayTracing();
        //

        struct DeviceHandle
        {
            vk::Device device;
            VmaAllocator allocator;
            vk::SurfaceKHR surface;
            vk::Instance instance;
            ~DeviceHandle()
            {
                while (!DeletionQueue::Main().IsEmpty() || !DeletionQueue::Frame().IsEmpty())
                {
                    DeletionQueue::Frame().Flush();
                    DeletionQueue::Main().Flush();
                }
                device.waitIdle();
                vmaDestroyAllocator(allocator);
                device.destroy();
                VulkanGraphicsDevice::s_Instance = nullptr;
                instance.destroySurfaceKHR(surface);
            }
        };
        QueueFamilyIndices m_QueueFamilyIndices;
        SwapChainSupportDetails m_SwapChainSupportDetails;

        void CreateSwapChainSupportDetails();

        DeviceHandle m_DeviceHandle;
        Scope<VulkanSwapChain> m_SwapChain;
        vk::Device m_Device;
        vk::PhysicalDevice m_PhysicalDevice;
        uint64_t m_VRAM = 0;
        vk::Queue m_GraphicsQueue;
        vk::Queue m_PresentQueue;

        // Ref<VulkanPipeline> m_Pipeline;
        vk::CommandBuffer m_MainCommandBuffer;
        vk::CommandPool m_CommandPool;
        vk::CommandBufferAllocateInfo m_CommandPoolAllocateInfo;

        struct DeletionQueueFlusher
        {
            ~DeletionQueueFlusher() { DeletionQueue::Main().Flush(); }
        };

        // DeletionQueueFlusher m_Deleter;

        bool m_SwapChainRebuildRequested = false;

        void LogDeviceProperties(vk::PhysicalDevice& device) const;
        QueueFamilyIndices FindQueueFamilies();
        GPU SelectPhysicalDevice(const VulkanInstance& instance);
        void CreateLogicalDevice(GPU gpu);
        void CreateSurface(VulkanInstance& instance);
        void InitializeVulkanMemoryAllocator(VulkanInstance& instance);

        friend VmaAllocator GetVulkanAllocator();
    };

    inline VmaAllocator GetVulkanAllocator()
    {
        static auto allocator = VulkanGraphicsDevice::s_Instance->m_DeviceHandle.allocator;
        return allocator;
    }
} // namespace BeeEngine::Internal
#endif
