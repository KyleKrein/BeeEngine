//
// Created by alexl on 09.06.2023.
//

#include "VulkanGraphicsDevice.h"
#include "Renderer/QueueFamilyIndices.h"
#include <set>


namespace BeeEngine::Internal
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(VulkanInstance &instance)
    {
        m_Surface = instance.CreateSurface();
        CreatePhysicalDevice(instance);
        CreateLogicalDevice();
        m_GraphicsQueue->Initialize(m_PhysicalDevice, m_Device);
        if(m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            m_PresentQueue->Initialize(m_PhysicalDevice, m_Device);
        }
        m_SwapChain = CreateRef<VulkanSwapChain>(m_PhysicalDevice, m_Device, m_Surface->GetHandle(), WindowHandler::GetInstance()->GetWidth(),WindowHandler::GetInstance()->GetHeight() ,m_QueueFamilyIndices);
        GraphicsPipelineInBundle pipelineInBundle {m_SwapChain->GetExtent().width,
                                                   m_SwapChain->GetExtent().height,
                                                   m_SwapChain->GetFormat(),
                                                   CreateRef<VulkanShaderModule>("TestVertex", "shaders/TestVertex.spv"),
                                                   CreateRef<VulkanShaderModule>("TestFragment", "shaders/TestFragment.spv")};
        m_Pipeline = CreateRef<VulkanPipeline>(pipelineInBundle);
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        m_Device.destroy();
    }

    void VulkanGraphicsDevice::LogDeviceProperties(vk::PhysicalDevice &device) const
    {
        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

        BeeCoreInfo("Device name: {}", deviceProperties.deviceName);
        BeeCoreInfo("Device type: {}", vk::to_string(deviceProperties.deviceType));
        BeeCoreInfo("Device driver version: {}", deviceProperties.driverVersion);
        BeeCoreInfo("Device API version: {}", deviceProperties.apiVersion);
        BeeCoreInfo("Device vendor ID: {}", deviceProperties.vendorID);
        BeeCoreInfo("Device device ID: {}", deviceProperties.deviceID);
    }

    bool VulkanGraphicsDevice::IsSuitableDevice(const vk::PhysicalDevice &device) const
    {
        const std::vector<const char *> requiredExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        if(CheckDeviceExtensionSupport(device, requiredExtensions))
        {
            BeeCoreInfo("Device {} supports all required extensions", device.getProperties().deviceName);
        }
        else
        {
            BeeCoreInfo("Device {} does not support all required extensions", device.getProperties().deviceName);
            return false;
        }

        return true;
    }

    bool VulkanGraphicsDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice &device,
                                                           const std::vector<const char *> &extensions) const
    {
        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

        std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

        for (const auto& extension:availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices VulkanGraphicsDevice::FindQueueFamilies()
    {
        QueueFamilyIndices indices;

        std::vector<vk::QueueFamilyProperties> queueFamilies = m_PhysicalDevice.getQueueFamilyProperties();

        BeeCoreInfo("System can support {} queue families", queueFamilies.size());
        uint32_t i = 0;
        for (auto& queueFamily : queueFamilies)
        {
            if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                BeeCoreInfo("Queue family {} supports graphics operations", queueFamily.queueCount);
                indices.GraphicsFamily = i;
            }
            i++;
        }
        if(m_PhysicalDevice.getSurfaceSupportKHR(indices.GraphicsFamily.value(), m_Surface->GetHandle()))
        {
            BeeCoreInfo("Queue family {} supports presentation", indices.GraphicsFamily.value());
            indices.PresentFamily = indices.GraphicsFamily;
        }

        if(!indices.IsComplete())
        {
            BeeCoreError("Could not find suitable queue family");
        }
        return indices;
    }

    void VulkanGraphicsDevice::CreatePhysicalDevice(const VulkanInstance &instance)
    {
        /*
         * Choose a suitable physical device from a list of available devices
         * Note: Physical devices are the actual hardware (GPUs) that are installed on the system
         * They are neither created nor destroyed by the application
         */

        // Get a list of available physical devices
        std::vector<vk::PhysicalDevice> physicalDevices = instance.GetHandle().enumeratePhysicalDevices();

        BeeCoreInfo("There are {} physical devices available", physicalDevices.size());

        for (auto& device:physicalDevices)
        {
#if defined(DEBUG)
            LogDeviceProperties(device);
#endif
            if(IsSuitableDevice(device))
            {
                m_PhysicalDevice = device;
                break;
            }
        }

        if(!m_PhysicalDevice)
        {
            BeeCoreError("No suitable physical device found");
        }
    }

    void VulkanGraphicsDevice::CreateLogicalDevice()
    {
        m_QueueFamilyIndices = FindQueueFamilies();

        float queuePriority[] {1.0f};
        m_GraphicsQueue = CreateRef<VulkanGraphicsQueue>(m_QueueFamilyIndices.GraphicsFamily.value(), queuePriority);

        uint32_t numberOfQueuesToCreate = 1;

        if(m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            m_PresentQueue = CreateRef<VulkanGraphicsQueue>(m_QueueFamilyIndices.PresentFamily.value(), queuePriority);
            numberOfQueuesToCreate++;
        }
        else
        {
            m_PresentQueue = m_GraphicsQueue;
        }

        std::vector<const char*> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        vk::PhysicalDeviceFeatures deviceFeatures = {};

        std::vector<const char*> enabledLayers;

#if defined(BEE_VULKAN_ENABLE_VALIDATION_LAYERS)
        enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.push_back(m_GraphicsQueue->GetQueueCreateInfo());
        if(numberOfQueuesToCreate > 1)
        {
            queueCreateInfos.push_back(m_PresentQueue->GetQueueCreateInfo());
        }

        vk::DeviceCreateInfo deviceInfo(
                vk::DeviceCreateFlags(),
                numberOfQueuesToCreate, queueCreateInfos.data(),
                enabledLayers.size(), enabledLayers.data()
                , deviceExtensions.size(), deviceExtensions.data(),
                &deviceFeatures
                );

        try
        {
            m_Device = m_PhysicalDevice.createDevice(deviceInfo);
            BeeCoreInfo("Logical device created successfully");
            return;
        }
        catch (vk::SystemError& e)
        {
            BeeCoreError("Failed to create logical device: {}", e.what());
        }
    }
}