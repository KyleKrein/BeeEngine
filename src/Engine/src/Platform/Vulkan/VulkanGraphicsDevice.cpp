//
// Created by alexl on 09.06.2023.
//
#include <SDL_vulkan.h>
#if defined(BEE_COMPILE_VULKAN)
#include "VulkanGraphicsDevice.h"
#include "Renderer/QueueFamilyIndices.h"
#include <set>
#include "Core/Application.h"
#include "Core/DeletionQueue.h"
#include "Utils.h"

namespace BeeEngine::Internal
{
    const static std::vector<const char *> requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
};
    VulkanGraphicsDevice* VulkanGraphicsDevice::s_Instance = nullptr;
    PFN_vkCmdTraceRaysKHR CmdTraceRaysKHR = nullptr;
    PFN_vkDestroyAccelerationStructureKHR DestroyAccelerationStructureKHR = nullptr;
    PFN_vkGetRayTracingShaderGroupHandlesKHR GetRayTracingShaderGroupHandlesKHR = nullptr;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR CmdWriteAccelerationStructuresPropertiesKHR =
        nullptr;
    PFN_vkCreateAccelerationStructureKHR CreateAccelerationStructureKHR = nullptr;
    PFN_vkCmdBuildAccelerationStructuresKHR CmdBuildAccelerationStructuresKHR = nullptr;
    PFN_vkCmdCopyAccelerationStructureKHR CmdCopyAccelerationStructureKHR = nullptr;
    PFN_vkCreateRayTracingPipelinesKHR CreateRayTracingPipelinesKHR = nullptr;
    PFN_vkGetAccelerationStructureDeviceAddressKHR GetAccelerationStructureDeviceAddressKHR =
        nullptr;
    PFN_vkGetAccelerationStructureBuildSizesKHR GetAccelerationStructureBuildSizesKHR = nullptr;

    void VulkanGraphicsDevice::LoadKHRRayTracing()
    {
        VkDevice device = m_Device;
        CmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(
            vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));
        DestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
        GetRayTracingShaderGroupHandlesKHR =
            reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(
                vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
        CmdWriteAccelerationStructuresPropertiesKHR =
            reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(
                vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
        CreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
        CmdBuildAccelerationStructuresKHR =
            reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(
                vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));
        CmdCopyAccelerationStructureKHR = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureKHR"));
        CreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(
            vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));
        GetAccelerationStructureDeviceAddressKHR =
            reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(
                vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));
        GetAccelerationStructureBuildSizesKHR =
            reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(
                vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));
        if(!CmdTraceRaysKHR || !DestroyAccelerationStructureKHR || !GetRayTracingShaderGroupHandlesKHR
           || !CmdWriteAccelerationStructuresPropertiesKHR || !CreateAccelerationStructureKHR
           || !CmdBuildAccelerationStructuresKHR || !CmdCopyAccelerationStructureKHR
           || !CreateRayTracingPipelinesKHR || !GetAccelerationStructureDeviceAddressKHR
           || !GetAccelerationStructureBuildSizesKHR)
        {
            BeeCoreError("Failed to load KHR Ray Tracing functions");
        }
        else
        {
            BeeCoreTrace("Loaded KHR Ray Tracing functions");
        }
    }
    VulkanGraphicsDevice::VulkanGraphicsDevice(VulkanInstance &instance)
    {
        if(s_Instance != nullptr)
        {
            BeeCoreFatalError("Can't create two graphics devices at once");
        }
        s_Instance = this;
        CreateSurface(instance);
        SelectPhysicalDevice(instance);
        CreateLogicalDevice();

        LoadKHRRayTracing();

        InitializeVulkanMemoryAllocator(instance);

        m_GraphicsQueue = m_Device.getQueue(m_QueueFamilyIndices.GraphicsFamily.value(), 0);
        if(m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            m_PresentQueue = m_Device.getQueue(m_QueueFamilyIndices.PresentFamily.value(), 0);
        }
        CreateSwapChainSupportDetails();
        m_SwapChain = CreateScope<VulkanSwapChain>(*this, WindowHandler::GetInstance()->GetWidth(),WindowHandler::GetInstance()->GetHeight());
        CreateCommandPool();
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        m_Device.waitIdle();
        //DeletionQueue::Main().Flush();
    }

    void VulkanGraphicsDevice::LogDeviceProperties(vk::PhysicalDevice &device) const
    {
        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

        BeeCoreInfo("Device name: {}", deviceProperties.deviceName.data());
        BeeCoreInfo("Device type: {}", vk::to_string(deviceProperties.deviceType));
        BeeCoreInfo("Device driver version: {}", deviceProperties.driverVersion);
        BeeCoreInfo("Device API version: {}", deviceProperties.apiVersion);
        BeeCoreInfo("Device vendor ID: {}", deviceProperties.vendorID);
        BeeCoreInfo("Device device ID: {}", deviceProperties.deviceID);
    }

    bool VulkanGraphicsDevice::IsSuitableDevice(const vk::PhysicalDevice &device) const
    {
        if(CheckDeviceExtensionSupport(device, requiredExtensions))
        {
            BeeCoreInfo("Device {} supports all required extensions", device.getProperties().deviceName.data());
        }
        else
        {
            BeeCoreInfo("Device {} does not support all required extensions", device.getProperties().deviceName.data());
            return false;
        }

        return true;
    }

    bool VulkanGraphicsDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice &device,
                                                           const std::vector<const char *> &extensions) const
    {
        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

        std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

        BeeCoreTrace("Required Extensions for Graphics Device:");
        for (const auto& extension : requiredExtensions)
        {
            BeeCoreTrace("{}", extension);
        }

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
                BeeCoreInfo("Queue family {} supports graphics operations", i);
                indices.GraphicsFamily = i;
                break;
            }
            i++;
        }
        if(m_PhysicalDevice.getSurfaceSupportKHR(indices.GraphicsFamily.value(), m_Surface))
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

    void VulkanGraphicsDevice::SelectPhysicalDevice(const VulkanInstance &instance)
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
        vk::DeviceQueueCreateInfo graphicsQueueCreateInfo = {
            vk::DeviceQueueCreateFlags(),
            m_QueueFamilyIndices.GraphicsFamily.value(),
            1,
            queuePriority};
        vk::DeviceQueueCreateInfo presentQueueCreateInfo = graphicsQueueCreateInfo;

        uint32_t numberOfQueuesToCreate = 1;

        if(m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            presentQueueCreateInfo = {
                vk::DeviceQueueCreateFlags(),
                m_QueueFamilyIndices.PresentFamily.value(),
                1,
                queuePriority};
            numberOfQueuesToCreate++;
        }
        else
        {
            m_PresentQueue = m_GraphicsQueue;
        }

        std::vector<const char*> deviceExtensions = requiredExtensions;

        if(BeeEngine::Application::GetOsPlatform() == OSPlatform::Mac)
        {
            deviceExtensions.push_back("VK_KHR_portability_subset");
        }

        vk::PhysicalDeviceFeatures deviceFeatures = {};

        std::vector<const char*> enabledLayers;

#if defined(BEE_VULKAN_ENABLE_VALIDATION_LAYERS)
        enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.push_back(graphicsQueueCreateInfo);
        if(numberOfQueuesToCreate > 1)
        {
            queueCreateInfos.push_back(presentQueueCreateInfo);
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

    void VulkanGraphicsDevice::CreateSurface(VulkanInstance& instance)
    {
        VkSurfaceKHR cSurface;
        auto sdlWindow = (SDL_Window*)WindowHandler::GetInstance()->GetWindow();
        auto result = SDL_Vulkan_CreateSurface(sdlWindow, instance.GetHandle(), nullptr, &cSurface);
        if(result != SDL_TRUE)
        {
            BeeCoreFatalError("Failed to create Vulkan surface!");
        }
        m_Surface = vk::SurfaceKHR(cSurface);
    }

    VulkanBuffer VulkanGraphicsDevice::CreateBuffer(vk::DeviceSize size,
                                            vk::BufferUsageFlags usage,
                                            VmaMemoryUsage memoryUsage) const
    {
        //allocate vertex buffer
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //this is the total size, in bytes, of the buffer we are allocating
        bufferInfo.size = size;
        //this buffer is going to be used as a Vertex Buffer
        bufferInfo.usage = (VkBufferUsageFlags)usage;
        //bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


        //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = memoryUsage;
        if(memoryUsage == VMA_MEMORY_USAGE_AUTO)
        {
            vmaallocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        //allocate the buffer
        VulkanBuffer buffer;
        auto result = vmaCreateBuffer(m_DeviceHandle.allocator, &bufferInfo, &vmaallocInfo,
                                      (VkBuffer*)&buffer.Buffer,
                                      &buffer.Memory,
                                      nullptr);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer!");
        }

        //add the destruction of mesh buffer to the deletion queue
        DeletionQueue::Main().PushFunction([buffer]() {
            vmaDestroyBuffer(GetVulkanAllocator(), buffer.Buffer, buffer.Memory);
        });
        return buffer;
    }

    void VulkanGraphicsDevice::CopyToBuffer(gsl::span<byte> data, VulkanBuffer& outBuffer) const
    {
        void* mappedData;
        vmaMapMemory(m_DeviceHandle.allocator, outBuffer.Memory, &mappedData);

        memcpy(mappedData, data.data(), data.size());

        vmaUnmapMemory(m_DeviceHandle.allocator, outBuffer.Memory);
    }

    vk::CommandBuffer VulkanGraphicsDevice::BeginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = m_CommandPool;
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

        m_GraphicsQueue.submit(1, &submitInfo, nullptr);
        m_GraphicsQueue.waitIdle();

        m_Device.freeCommandBuffers(m_CommandPool, 1, &commandBuffer);
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
    VulkanGraphicsDevice::CreateImageWithInfo(const vk::ImageCreateInfo& imageInfo,
                 const vk::ImageViewCreateInfo& imageViewInfo,
                 vk::MemoryPropertyFlags memoryProperties,
                 const VmaMemoryUsage& memoryUsage,
                 VulkanImage& outImage,
                 vk::ImageView& outImageView) const
    {
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = memoryUsage;
        allocInfo.requiredFlags = (VkMemoryPropertyFlags)memoryProperties;

        //allocate and create the image
        vmaCreateImage(m_DeviceHandle.allocator, (VkImageCreateInfo*)&imageInfo, &allocInfo, (VkImage*)&outImage.Image, &outImage.Memory, nullptr);
        auto copiedImageViewInfo = imageViewInfo;
        copiedImageViewInfo.image = outImage.Image;
        auto result = m_Device.createImageView(&copiedImageViewInfo, nullptr, &outImageView);

        if(result != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to create image view!");
        }
        //add to deletion queues
        DeletionQueue::Main().PushFunction([device = m_Device, allocator = m_DeviceHandle.allocator, outImageView, outImage]() {
            vkDestroyImageView(device, outImageView, nullptr);
            vmaDestroyImage(allocator, outImage.Image, outImage.Memory);
        });


        /*vkCreateImage(m_Device, &imageInfo, nullptr, &image);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
        vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory);

        m_Device.bindImageMemory(image, imageMemory, 0);*/
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
        m_SwapChainSupportDetails.capabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);

        uint32_t formatCount;
        m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            m_SwapChainSupportDetails.formats.resize(formatCount);
            m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface, &formatCount, m_SwapChainSupportDetails.formats.data());
        }

        uint32_t presentModeCount;
        m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            m_SwapChainSupportDetails.presentModes.resize(presentModeCount);
            m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface, &presentModeCount, m_SwapChainSupportDetails.presentModes.data());
        }
    }

    vk::Format VulkanGraphicsDevice::FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
                                                       vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates) {
            vk::FormatProperties props;
            m_PhysicalDevice.getFormatProperties(format, &props);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == vk::ImageTiling::eOptimal &&
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
        auto oldSwapChain = m_SwapChain->GetHandle();
        CreateSwapChainSupportDetails();
        m_SwapChain = CreateScope<VulkanSwapChain>(*this, width, height, oldSwapChain);
        m_SwapChainRebuildRequested = false;
        BeeEnsures(m_SwapChain->GetHandle() != VK_NULL_HANDLE);
        BeeEnsures(m_SwapChain->GetHandle() != oldSwapChain);
    }

    VulkanGraphicsDevice &VulkanGraphicsDevice::GetInstance()
    {
        return *s_Instance;
    }

    void VulkanGraphicsDevice::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily.value();

        try
        {
            m_CommandPool = m_Device.createCommandPool(commandPoolCreateInfo);
        }
        catch (vk::SystemError &e)
        {
            BeeCoreError("Failed to create command pool: {0}", e.what());
        }

        m_CommandPoolAllocateInfo.commandPool = m_CommandPool;
        m_CommandPoolAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        m_CommandPoolAllocateInfo.commandBufferCount = 1;
    }

    /*void VulkanGraphicsDevice::UploadMesh(Mesh& mesh) const
    {
        //allocate vertex buffer
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //this is the total size, in bytes, of the buffer we are allocating
        bufferInfo.size = mesh.Vertices.size() * sizeof(Vertex);
        //this buffer is going to be used as a Vertex Buffer
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


        //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        //allocate the buffer
        auto result = vmaCreateBuffer(m_DeviceHandle.allocator, &bufferInfo, &vmaallocInfo,
                                 &mesh.VertexBuffer.Buffer,
                                 &mesh.VertexBuffer.Memory,
                                 nullptr);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate vertex buffer!");
        }

        //add the destruction of mesh buffer to the deletion queue
        DeletionQueue::Main().PushFunction([=]() {

            vmaDestroyBuffer(GetVulkanAllocator(), mesh.VertexBuffer.Buffer, mesh.VertexBuffer.Memory);
        });

        //copy vertex data
        void* data;
        vmaMapMemory(m_DeviceHandle.allocator, mesh.VertexBuffer.Memory, &data);

        memcpy(data, mesh.Vertices.data(), mesh.Vertices.size() * sizeof(Vertex));

        vmaUnmapMemory(m_DeviceHandle.allocator, mesh.VertexBuffer.Memory);

    }*/
}
#endif