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

    private:
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_Device;
        Ref<VulkanGraphicsQueue> m_GraphicsQueue;
        Ref<VulkanGraphicsQueue> m_PresentQueue;
        Ref<VulkanSurface> m_Surface;
        Ref<VulkanSwapChain> m_SwapChain;
        Ref<VulkanPipeline> m_Pipeline;
        QueueFamilyIndices m_QueueFamilyIndices;

        void LogDeviceProperties(vk::PhysicalDevice &device) const;

        bool IsSuitableDevice(const vk::PhysicalDevice &device) const;

        bool CheckDeviceExtensionSupport(const vk::PhysicalDevice &device,
                                         const std::vector<const char *> &extensions) const;

        QueueFamilyIndices FindQueueFamilies();
        void CreatePhysicalDevice(const VulkanInstance &instance);
        void CreateLogicalDevice();
    };
}
