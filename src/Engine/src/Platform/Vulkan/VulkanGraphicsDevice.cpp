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
        /*
        GraphicsPipelineInBundle pipelineInBundle {m_SwapChain->GetExtent().width,
                                                   m_SwapChain->GetExtent().height,
                                                   m_SwapChain->GetFormat(),
                                                   CreateRef<VulkanShaderModule>(m_Device, "TestVertex", "shaders/vertex.spv"),
                                                   CreateRef<VulkanShaderModule>(m_Device, "TestFragment", "shaders/fragment.spv")};
        */
         //m_Pipeline = CreateRef<VulkanPipeline>(m_Device, pipelineInBundle);
        //VulkanFramebuffer::CreateFramebuffers(m_Device, m_Pipeline->GetRenderPass().GetHandle(), m_SwapChain->GetExtent(), m_SwapChain->GetFrames());
        //m_CommandPool = CreateRef<VulkanCommandPool>(m_Device, m_QueueFamilyIndices);
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

    void VulkanGraphicsDevice::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                                            vk::MemoryPropertyFlags properties, vk::Buffer &buffer,
                                            vk::DeviceMemory &bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        m_Device.createBuffer(&bufferInfo, nullptr, &buffer);

        vk::MemoryRequirements memRequirements;
        m_Device.getBufferMemoryRequirements(buffer, &memRequirements);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        m_Device.allocateMemory(&allocInfo, nullptr, &bufferMemory);

        m_Device.bindBufferMemory(buffer, bufferMemory, 0);
    }

    vk::CommandBuffer VulkanGraphicsDevice::BeginSingleTimeCommands()
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

    void VulkanGraphicsDevice::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        m_GraphicsQueue->GetQueue().submit(1, &submitInfo, VK_NULL_HANDLE);
        m_GraphicsQueue->GetQueue().waitIdle();

        m_Device.freeCommandBuffers(m_CommandPool->GetHandle(), 1, &commandBuffer);
    }

    void VulkanGraphicsDevice::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::BufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanGraphicsDevice::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height,
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
    VulkanGraphicsDevice::CreateImageWithInfo(const vk::ImageCreateInfo &imageInfo, vk::MemoryPropertyFlags properties,
                                              vk::Image &image, vk::DeviceMemory &imageMemory)
    {
        m_Device.createImage(&imageInfo, nullptr, &image);

        vk::MemoryRequirements memRequirements;
        m_Device.getImageMemoryRequirements(image, &memRequirements);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        m_Device.allocateMemory(&allocInfo, nullptr, &imageMemory);

        m_Device.bindImageMemory(image, imageMemory, 0);
    }

    uint32_t VulkanGraphicsDevice::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties;
        m_PhysicalDevice.getMemoryProperties(&memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
}
