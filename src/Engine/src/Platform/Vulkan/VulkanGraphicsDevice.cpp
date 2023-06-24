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
        InitializeVulkanMemoryAllocator(instance);
        m_GraphicsQueue->Initialize(m_PhysicalDevice, m_Device);
        if(m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            m_PresentQueue->Initialize(m_PhysicalDevice, m_Device);
        }
        CreateSwapChainSupportDetails();
        m_SwapChain = CreateScope<VulkanSwapChain>(*this, WindowHandler::GetInstance()->GetWidth(),WindowHandler::GetInstance()->GetHeight(), nullptr);
        /*
        GraphicsPipelineInBundle pipelineInBundle {m_SwapChain->GetExtent().width,
                                                   m_SwapChain->GetExtent().height,
                                                   m_SwapChain->GetFormat(),
                                                   CreateRef<VulkanShaderModule>(m_Device, "TestVertex", "shaders/vertex.spv"),
                                                   CreateRef<VulkanShaderModule>(m_Device, "TestFragment", "shaders/fragment.spv")};
        */
         //m_Pipeline = CreateRef<VulkanPipeline>(m_Device, pipelineInBundle);
        //VulkanFramebuffer::CreateFramebuffers(m_Device, m_Pipeline->GetRenderPass().GetHandle(), m_SwapChain->GetExtent(), m_SwapChain->GetFrames());
        m_CommandPool = CreateRef<VulkanCommandPool>(m_Device, m_QueueFamilyIndices);
        //m_CommandPool->CreateCommandBuffers(m_SwapChain->GetFrames());
        //m_MainCommandBuffer = m_CommandPool->CreateCommandBuffer();
        /*
        for(auto& frame : m_SwapChain->GetFrames())
        {
            frame.ImageAvailableSemaphore = VulkanSemaphore(m_Device);
            frame.RenderFinishedSemaphore = VulkanSemaphore(m_Device);
            frame.InFlightFence = VulkanFence(m_Device);
        }
         */
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        m_Device.waitIdle();
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
            m_DeviceHandle.device = m_Device;
            return;
        }
        catch (vk::SystemError& e)
        {
            BeeCoreError("Failed to create logical device: {}", e.what());
        }
    }

    void VulkanGraphicsDevice::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                            VkMemoryPropertyFlags properties, VkBuffer &buffer,
                                            VkDeviceMemory &bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocationInfo.requiredFlags = properties;

        VmaAllocation allocation;
        //vmaCreateBuffer(m_DeviceHandle.allocator, &bufferInfo, &allocationInfo, &buffer, &allocation, nullptr);

        vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer);

        vk::MemoryRequirements memRequirements;
        m_Device.getBufferMemoryRequirements(buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocCreateInfo.requiredFlags = properties;

        //vmaAllocateMemory(m_DeviceHandle.allocator, &memRequirements, &allocCreateInfo, &allocation, &bufferMemory, nullptr);

        vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory);

        m_Device.bindBufferMemory(buffer, bufferMemory, 0);
    }

    VkCommandBuffer VulkanGraphicsDevice::BeginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = m_CommandPool->GetHandle();
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer;
        m_Device.allocateCommandBuffers(&allocInfo, &commandBuffer);

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(&beginInfo);
        return commandBuffer;
    }

    void VulkanGraphicsDevice::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_GraphicsQueue->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_GraphicsQueue->GetQueue());

        vkFreeCommandBuffers(m_Device, m_CommandPool->GetHandle(), 1, &commandBuffer);
    }

    void VulkanGraphicsDevice::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanGraphicsDevice::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                                                 uint32_t layerCount)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = vk::Offset3D{0, 0, 0};
        region.imageExtent = vk::Extent3D{width, height, 1};

        commandBuffer.copyBufferToImage(
                buffer,
                image,
                vk::ImageLayout::eTransferDstOptimal,
                1,
                &region);
        EndSingleTimeCommands(commandBuffer);
    }

    void
    VulkanGraphicsDevice::CreateImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties,
                                              VkImage &image, VkDeviceMemory &imageMemory)
    {
        vkCreateImage(m_Device, &imageInfo, nullptr, &image);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
        vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory);

        m_Device.bindImageMemory(image, imageMemory, 0);
    }

    uint32_t VulkanGraphicsDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void VulkanGraphicsDevice::CreateSwapChainSupportDetails()
    {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface->GetHandle(), &m_SwapChainSupportDetails.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface->GetHandle(), &formatCount, nullptr);

        if (formatCount != 0) {
            m_SwapChainSupportDetails.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface->GetHandle(), &formatCount, m_SwapChainSupportDetails.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface->GetHandle(), &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            m_SwapChainSupportDetails.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                    m_PhysicalDevice,
                    m_Surface->GetHandle(),
                    &presentModeCount,
                    m_SwapChainSupportDetails.presentModes.data());
        }
    }

    VkFormat VulkanGraphicsDevice::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                                       VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                       (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    void VulkanGraphicsDevice::InitializeVulkanMemoryAllocator(VulkanInstance &instance)
    {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorCreateInfo.physicalDevice = m_PhysicalDevice;
        allocatorCreateInfo.device = m_Device;
        allocatorCreateInfo.instance = instance.GetHandle();
        allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
        vmaCreateAllocator(&allocatorCreateInfo, &m_DeviceHandle.allocator);
    }

    void VulkanGraphicsDevice::WindowResized(uint32_t width, uint32_t height)
    {
        m_SwapChain = CreateScope<VulkanSwapChain>(*this, width, height, std::move(m_SwapChain));
    }
}
