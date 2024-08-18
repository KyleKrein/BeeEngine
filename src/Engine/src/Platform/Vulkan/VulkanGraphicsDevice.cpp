//
// Created by alexl on 09.06.2023.
//
// clang-format off

#include "Gui/MessageBox.h"
#if defined(BEE_COMPILE_VULKAN)
#include "Platform/ImGui/ImGuiControllerVulkan.h"
#if defined(WINDOWS)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif
#include <vulkan/vulkan.hpp>
#if defined(BEE_COMPILE_SDL)
#include <SDL_vulkan.h>
#endif
#include "VulkanGraphicsDevice.h"
#include "Renderer/QueueFamilyIndices.h"
#include <set>
#include "Core/Application.h"
#include "Core/DeletionQueue.h"
#include "Utils.h"

// clang-format on
namespace BeeEngine::Internal
{
    static std::vector<const char*> requiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                          VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                                                          VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                                                          VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                                                          VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                                                          VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                                                          VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME};
    VulkanGraphicsDevice* VulkanGraphicsDevice::s_Instance = nullptr;
    PFN_vkCmdTraceRaysKHR CmdTraceRaysKHR = nullptr;
    PFN_vkDestroyAccelerationStructureKHR DestroyAccelerationStructureKHR = nullptr;
    PFN_vkGetRayTracingShaderGroupHandlesKHR GetRayTracingShaderGroupHandlesKHR = nullptr;
    PFN_vkCmdWriteAccelerationStructuresPropertiesKHR CmdWriteAccelerationStructuresPropertiesKHR = nullptr;
    PFN_vkCreateAccelerationStructureKHR CreateAccelerationStructureKHR = nullptr;
    PFN_vkCmdBuildAccelerationStructuresKHR CmdBuildAccelerationStructuresKHR = nullptr;
    PFN_vkCmdCopyAccelerationStructureKHR CmdCopyAccelerationStructureKHR = nullptr;
    PFN_vkCreateRayTracingPipelinesKHR CreateRayTracingPipelinesKHR = nullptr;
    PFN_vkGetAccelerationStructureDeviceAddressKHR GetAccelerationStructureDeviceAddressKHR = nullptr;
    PFN_vkGetAccelerationStructureBuildSizesKHR GetAccelerationStructureBuildSizesKHR = nullptr;

    static std::atomic<size_t> VulkanBufferCount = 0;
    static std::atomic<size_t> VulkanImageCount = 0;
    void VulkanGraphicsDevice::LoadKHRRayTracing()
    {
        VkDevice device = m_Device;
        CmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));
        DestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
        GetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(
            vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
        CmdWriteAccelerationStructuresPropertiesKHR =
            reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(
                vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
        CreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
        CmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(
            vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));
        CmdCopyAccelerationStructureKHR = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(
            vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureKHR"));
        CreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(
            vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));
        GetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(
            vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));
        GetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(
            vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));
        if (!CmdTraceRaysKHR || !DestroyAccelerationStructureKHR || !GetRayTracingShaderGroupHandlesKHR ||
            !CmdWriteAccelerationStructuresPropertiesKHR || !CreateAccelerationStructureKHR ||
            !CmdBuildAccelerationStructuresKHR || !CmdCopyAccelerationStructureKHR || !CreateRayTracingPipelinesKHR ||
            !GetAccelerationStructureDeviceAddressKHR || !GetAccelerationStructureBuildSizesKHR)
        {
            BeeCoreError("Failed to load KHR Ray Tracing functions");
        }
        else
        {
            BeeCoreTrace("Loaded KHR Ray Tracing functions");
        }
    }
    VulkanGraphicsDevice::VulkanGraphicsDevice(VulkanInstance& instance)
    {
        if (s_Instance != nullptr)
        {
            BeeCoreFatalError("Can't create two graphics devices at once");
        }
        s_Instance = this;
        m_DeviceHandle.instance = instance.GetHandle();
        CreateSurface(instance);
        SelectPhysicalDevice(instance);
        CreateLogicalDevice();

        DeletionQueue::Main().PushFunction([device = m_Device] { device.waitIdle(); });
        if (HasRayTracingSupport())
            LoadKHRRayTracing();

        InitializeVulkanMemoryAllocator(instance);

        g_vkDynamicLoader.init(instance.GetHandle(), m_Device);

        m_GraphicsQueue = m_Device.getQueue(m_QueueFamilyIndices.GraphicsFamily.value(), 0);
        if (m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            m_PresentQueue = m_Device.getQueue(m_QueueFamilyIndices.PresentFamily.value(), 0);
        }
        else
        {
            m_PresentQueue = m_GraphicsQueue;
        }
        CreateSwapChainSupportDetails();
        m_SwapChain = CreateScope<VulkanSwapChain>(
            *this, WindowHandler::GetInstance()->GetWidth(), WindowHandler::GetInstance()->GetHeight());
        CreateCommandPool();
        CreateDescriptorPool();
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        m_Device.waitIdle();
        while (!DeletionQueue::Main().IsEmpty() || !DeletionQueue::Frame().IsEmpty())
        {
            DeletionQueue::Frame().Flush();
            DeletionQueue::Main().Flush();
        }
        ImGuiControllerVulkan::s_ShutdownFunction();
        m_Device.waitIdle();
        m_Device.destroyDescriptorPool(m_DescriptorPool);
        m_Device.destroyCommandPool(m_CommandPool);
    }

    void VulkanGraphicsDevice::LogDeviceProperties(vk::PhysicalDevice& device) const
    {
        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

        BeeCoreInfo("Device name: {}", deviceProperties.deviceName.data());
        BeeCoreInfo("Device type: {}", vk::to_string(deviceProperties.deviceType));
        BeeCoreInfo("Device driver version: {}", deviceProperties.driverVersion);
        BeeCoreInfo("Device API version: {}", deviceProperties.apiVersion);
        BeeCoreInfo("Device vendor ID: {}", deviceProperties.vendorID);
        BeeCoreInfo("Device device ID: {}", deviceProperties.deviceID);
    }

    bool VulkanGraphicsDevice::CheckDeviceFeaturesSupport(const vk::PhysicalDevice& device) const
    {
        vk::PhysicalDeviceFeatures2 deviceFeatures2;
        vk::PhysicalDeviceVulkan11Features deviceVulkan11Features;
        vk::PhysicalDeviceVulkan12Features deviceVulkan12Features;
        vk::PhysicalDeviceVulkan13Features deviceVulkan13Features;
        deviceFeatures2.pNext = &deviceVulkan11Features;
        deviceVulkan11Features.pNext = &deviceVulkan12Features;
        deviceVulkan12Features.pNext = &deviceVulkan13Features;
        vkGetPhysicalDeviceFeatures2(device, (VkPhysicalDeviceFeatures2*)&deviceFeatures2);

        if (deviceFeatures2.features.samplerAnisotropy == vk::True &&
            deviceVulkan12Features.bufferDeviceAddress == vk::True &&
#if !defined(MACOS) && !defined(IOS)
            deviceVulkan13Features.dynamicRendering == vk::True &&
            deviceVulkan13Features.synchronization2 == vk::True &&
#endif
            deviceVulkan12Features.descriptorIndexing == vk::True)
        {
            return true;
        }
#if !defined(MACOS) && !defined(IOS)
        if (deviceVulkan13Features.synchronization2 != vk::True)
        {
            BeeCoreError("Device does not support synchronization2");
        }
        if (deviceVulkan13Features.dynamicRendering != vk::True)
        {
            BeeCoreError("Device does not support dynamic rendering");
        }
#endif
        if (deviceVulkan12Features.descriptorIndexing != vk::True)
        {
            BeeCoreError("Device does not support descriptor indexing");
        }
        if (deviceVulkan12Features.bufferDeviceAddress != vk::True)
        {
            BeeCoreError("Device does not support buffer device address");
        }
        return false;
    }

    bool VulkanGraphicsDevice::IsSuitableDevice(const vk::PhysicalDevice& device) const
    {
        if (!CheckDeviceExtensionSupport(device, requiredExtensions))
        {
            BeeCoreInfo("Device {} does not support all required extensions", device.getProperties().deviceName.data());
            return false;
        }
        BeeCoreInfo("Device {} supports all required extensions", device.getProperties().deviceName.data());

        if (!CheckDeviceFeaturesSupport(device))
        {
            BeeCoreInfo("Device {} does not support all required features", device.getProperties().deviceName.data());
            return false;
        }
        BeeCoreInfo("Device {} supports all required features", device.getProperties().deviceName.data());
        return true;
    }

    bool IsRayTracingExtension(const std::string& ext)
    {
        return ext == VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME ||
               ext == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME ||
               ext == VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME || ext == VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME ||
               ext == VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME ||
               ext == VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME;
    }

    bool AreRayTracingOnlyExtensionsPresent(const std::set<std::string>& set)
    {
        return std::ranges::all_of(set, IsRayTracingExtension);
    }

    bool VulkanGraphicsDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice& device,
                                                           std::vector<const char*>& extensions) const
    {
        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

        std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

        BeeCoreTrace("Required Extensions for Graphics Device:");
        for (const auto& extension : requiredExtensions)
        {
            BeeCoreTrace("- {}", extension);
        }

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        if (!requiredExtensions.empty())
        {
            if (AreRayTracingOnlyExtensionsPresent(requiredExtensions))
            {
                m_HasRayTracingSupport = false;
                for (auto& ext : requiredExtensions)
                {
                    for (auto it = extensions.begin(); it != extensions.end();)
                    {
                        if (strcmp(*it, ext.c_str()) == 0)
                        {
                            it = extensions.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                }
                requiredExtensions.clear();
            }
        }
        else
        {
            m_HasRayTracingSupport = true;
        }

        for (auto& ext : requiredExtensions)
        {
            BeeCoreTrace("Extension: {} is unsupported", ext);
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
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                BeeCoreInfo("Queue family {} supports graphics operations", i);
                indices.GraphicsFamily = i;
                break;
            }
            i++;
        }
        if (m_PhysicalDevice.getSurfaceSupportKHR(indices.GraphicsFamily.value(), m_DeviceHandle.surface))
        {
            BeeCoreInfo("Queue family {} supports presentation", indices.GraphicsFamily.value());
            indices.PresentFamily = indices.GraphicsFamily;
        }

        if (!indices.IsComplete())
        {
            BeeCoreError("Could not find suitable queue family");
        }
        return indices;
    }

    void VulkanGraphicsDevice::SelectPhysicalDevice(const VulkanInstance& instance)
    {
        /*
         * Choose a suitable physical device from a list of available devices
         * Note: Physical devices are the actual hardware (GPUs) that are installed on the system
         * They are neither created nor destroyed by the application
         */

        // Get a list of available physical devices
        std::vector<vk::PhysicalDevice> physicalDevices = instance.GetHandle().enumeratePhysicalDevices();

        BeeCoreInfo("There are {} physical devices available", physicalDevices.size());

        struct DeviceScore
        {
            vk::PhysicalDevice device;
            uint64_t vram = 0;
        };
        DeviceScore bestDevice = {nullptr, 0};
        for (auto& device : physicalDevices)
        {
#if defined(DEBUG)
            LogDeviceProperties(device);
#endif
            if (IsSuitableDevice(device))
            {
                auto vram = device.getMemoryProperties().memoryHeaps[0].size;
                if (bestDevice.device == nullptr || vram > bestDevice.vram)
                {
                    bestDevice = {device, vram};
                }
            }
        }

        m_PhysicalDevice = bestDevice.device;
        m_VRAM = bestDevice.vram;

        if (!m_PhysicalDevice)
        {
            BeeCoreError("No suitable physical device found");
            ShowMessageBox("Unable to choose GPU",
                           "It's likely that your graphics driver is outdated. Please update it and try again",
                           MessageBoxType::Error);
            throw std::runtime_error("No suitable physical device found");
        }
    }

    void VulkanGraphicsDevice::CreateLogicalDevice()
    {
        m_QueueFamilyIndices = FindQueueFamilies();

        float queuePriority[]{1.0f};
        vk::DeviceQueueCreateInfo graphicsQueueCreateInfo = {
            vk::DeviceQueueCreateFlags(), m_QueueFamilyIndices.GraphicsFamily.value(), 1, queuePriority};
        vk::DeviceQueueCreateInfo presentQueueCreateInfo = graphicsQueueCreateInfo;

        uint32_t numberOfQueuesToCreate = 1;

        if (m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            presentQueueCreateInfo = {
                vk::DeviceQueueCreateFlags(), m_QueueFamilyIndices.PresentFamily.value(), 1, queuePriority};
            numberOfQueuesToCreate++;
        }
        else
        {
            m_PresentQueue = m_GraphicsQueue;
        }

        std::vector<const char*> deviceExtensions = requiredExtensions;

        if (BeeEngine::Application::GetOsPlatform() == OSPlatform::Mac)
        {
            deviceExtensions.push_back("VK_KHR_portability_subset");
        }

        vk::PhysicalDeviceVulkan11Features deviceVulkan11Features = {};
        vk::PhysicalDeviceVulkan12Features deviceVulkan12Features = {};
        vk::PhysicalDeviceVulkan13Features deviceVulkan13Features = {};

        deviceVulkan11Features.pNext = &deviceVulkan12Features;
        deviceVulkan12Features.pNext = &deviceVulkan13Features;

        vk::PhysicalDeviceFeatures2 deviceFeatures2 = {};
        deviceFeatures2.pNext = &deviceVulkan11Features;

        deviceFeatures2.features.samplerAnisotropy = vk::True;

        deviceVulkan12Features.bufferDeviceAddress = vk::True;
        deviceVulkan12Features.descriptorIndexing = vk::True;

        deviceVulkan13Features.synchronization2 = vk::True;
        deviceVulkan13Features.dynamicRendering = vk::True;

        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures = {};
        rayTracingFeatures.rayTracingPipeline = HasRayTracingSupport() ? vk::True : vk::False;

        deviceVulkan13Features.pNext = &rayTracingFeatures;

        vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
        accelerationStructureFeatures.accelerationStructure = HasRayTracingSupport() ? vk::True : vk::False;

        rayTracingFeatures.pNext = &accelerationStructureFeatures;
        if (!HasRayTracingSupport())
        {
            deviceVulkan13Features.pNext = nullptr;
        }

        std::vector<const char*> enabledLayers;

#if defined(BEE_VULKAN_ENABLE_VALIDATION_LAYERS)
        enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.push_back(graphicsQueueCreateInfo);
        if (numberOfQueuesToCreate > 1)
        {
            queueCreateInfos.push_back(presentQueueCreateInfo);
        }

        vk::DeviceCreateInfo deviceInfo(vk::DeviceCreateFlags(),
                                        numberOfQueuesToCreate,
                                        queueCreateInfos.data(),
                                        enabledLayers.size(),
                                        enabledLayers.data(),
                                        deviceExtensions.size(),
                                        deviceExtensions.data(),
                                        nullptr);
        deviceInfo.pNext = &deviceFeatures2;

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
        switch (WindowHandler::GetAPI())
        {
#if defined(BEE_COMPILE_SDL)
            case WindowHandlerAPI::SDL:
            {
                auto sdlWindow = (SDL_Window*)WindowHandler::GetInstance()->GetWindow();
                auto result = SDL_Vulkan_CreateSurface(sdlWindow, instance.GetHandle(), nullptr, &cSurface);
                if (result != SDL_TRUE)
                {
                    BeeCoreFatalError("Failed to create Vulkan surface!");
                }
            }
            break;
#endif
#if defined(WINDOWS)
            case WindowHandlerAPI::WinAPI:
            {
                auto nativeData = WindowHandler::GetInstance()->GetNativeInfo();
                VkWin32SurfaceCreateInfoKHR createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                createInfo.hinstance = (HINSTANCE)nativeData.instance;
                createInfo.hwnd = (HWND)nativeData.window;
                auto result = vkCreateWin32SurfaceKHR(instance.GetHandle(), &createInfo, nullptr, &cSurface);
                if (result != VK_SUCCESS)
                {
                    BeeCoreFatalError("Failed to create Vulkan surface!");
                }
            }
            break;
#endif
        }
        m_DeviceHandle.surface = vk::SurfaceKHR(cSurface);
    }

    bool IsDepthFormat(vk::Format format)
    {
        switch (format)
        {
            case vk::Format::eD32Sfloat:
                return true;
        }
        return false;
    }

    void VulkanGraphicsDevice::TransitionImageLayout(vk::Image image,
                                                     vk::Format format,
                                                     vk::ImageLayout oldLayout,
                                                     vk::ImageLayout newLayout)
    {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer;
        CheckVkResult(m_Device.allocateCommandBuffers(&allocInfo, &commandBuffer));

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        CheckVkResult(commandBuffer.begin(&beginInfo));

        TransitionImageLayout(commandBuffer, image, format, oldLayout, newLayout);

        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        m_GraphicsQueue.submit(submitInfo, vk::Fence(nullptr));
        m_GraphicsQueue.waitIdle();

        m_Device.freeCommandBuffers(m_CommandPool, commandBuffer);
    }

    void VulkanGraphicsDevice::TransitionImageLayout(
        vk::CommandBuffer cmd, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
    {
        vk::ImageMemoryBarrier2 barrier{};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask =
            IsDepthFormat(format) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        // vk::PipelineStageFlags2 sourceStage;
        // vk::PipelineStageFlags2 destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
            barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
                 newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
            barrier.dstStageMask =
                vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader;
            if (HasRayTracingSupport())
                barrier.dstStageMask |= vk::PipelineStageFlagBits2::eRayTracingShaderKHR;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
        {
            barrier.setSrcAccessMask({});
            barrier.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite);

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        }
        else if (oldLayout == vk::ImageLayout::eColorAttachmentOptimal && newLayout == vk::ImageLayout::ePresentSrcKHR)
        {
            barrier.setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite);
            barrier.setDstAccessMask(vk::AccessFlagBits2::eMemoryRead);

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;
        }
        else if (oldLayout == vk::ImageLayout::ePresentSrcKHR && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
        {
            barrier.setSrcAccessMask(vk::AccessFlagBits2::eMemoryRead);
            barrier.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite);

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined &&
                 newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.setSrcAccessMask({});
            barrier.setDstAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentRead |
                                     vk::AccessFlagBits2::eDepthStencilAttachmentWrite);

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eAttachmentOptimal)
        {
            barrier.setSrcAccessMask({});
            barrier.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite);

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.setSrcAccessMask({});
            barrier.setDstAccessMask(vk::AccessFlagBits2::eShaderRead);

            barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
        }
        else
        {
            static std::vector<std::pair<vk::ImageLayout, vk::ImageLayout>> unsupportedTransitions;
            auto pair = std::make_pair(oldLayout, newLayout);
            if (std::ranges::find(unsupportedTransitions, pair) == unsupportedTransitions.end())
            {
                unsupportedTransitions.push_back(pair);
                BeeCoreWarn("Unsupported image layout transition! From {} to {}. Fallback to default",
                            vk::to_string(oldLayout),
                            vk::to_string(newLayout));
            }
            barrier.srcStageMask = vk::PipelineStageFlagBits2::eAllCommands;
            barrier.srcAccessMask = vk::AccessFlagBits2::eMemoryWrite;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eAllCommands;
            barrier.dstAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
        }
        vk::DependencyInfo dependencyInfo{};
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &barrier;

        cmd.pipelineBarrier2(dependencyInfo, g_vkDynamicLoader);
    }

    void VulkanGraphicsDevice::CreateDescriptorSet(vk::DescriptorSetAllocateInfo& info,
                                                   vk::DescriptorSet* outDescriptorSet) const
    {
        info.descriptorPool = m_DescriptorPool;
        CheckVkResult(m_Device.allocateDescriptorSets(&info, outDescriptorSet));
    }

    void VulkanGraphicsDevice::DestroyDescriptorSet(vk::DescriptorSet descriptorSet) const
    {
        DeletionQueue::Frame().PushFunction([descriptorSet, pool = m_DescriptorPool, device = m_Device]()
                                            { device.freeDescriptorSets(pool, descriptorSet); });
    }

    VulkanBuffer VulkanGraphicsDevice::CreateBuffer(vk::DeviceSize size,
                                                    vk::BufferUsageFlags usage,
                                                    VmaMemoryUsage memoryUsage) const
    {
        // allocate vertex buffer
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        // this is the total size, in bytes, of the buffer we are allocating
        bufferInfo.size = size;
        // this buffer is going to be used as a Vertex Buffer
        bufferInfo.usage = (VkBufferUsageFlags)usage;
        // bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // let the VMA library know that this data should be writeable by CPU, but also readable by GPU
        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = memoryUsage;
        // vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        if (memoryUsage == VMA_MEMORY_USAGE_AUTO)
        {
            vmaallocInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        // allocate the buffer
        VulkanBuffer buffer;
        auto result = vmaCreateBuffer(m_DeviceHandle.allocator,
                                      &bufferInfo,
                                      &vmaallocInfo,
                                      (VkBuffer*)&buffer.Buffer,
                                      &buffer.Memory,
                                      &buffer.Info);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer!");
        }
        BeeCoreTrace("Allocated Vulkan VMA buffer successfully! Buffer count: {}", ++VulkanBufferCount);
        return buffer;
    }

    void VulkanGraphicsDevice::DestroyBuffer(VulkanBuffer& buffer) const
    {
        DeletionQueue::Frame().PushFunction(
            [buf = buffer]()
            {
                vmaDestroyBuffer(GetVulkanAllocator(), buf.Buffer, buf.Memory);
                BeeCoreTrace("Destroyed Vulkan VMA buffer successfully! Buffer count: {}", --VulkanBufferCount);
            });
    }

    void VulkanGraphicsDevice::DestroyImage(VulkanImage& image) const
    {
        DeletionQueue::Frame().PushFunction(
            [im = image]()
            {
                vmaDestroyImage(GetVulkanAllocator(), im.Image, im.Memory);
                BeeCoreTrace("Destroyed Vulkan VMA Image successfully! Image count: {}", --VulkanImageCount);
            });
    }

    void VulkanGraphicsDevice::DestroyImageWithView(VulkanImage& image, vk::ImageView imageView) const
    {
        DeletionQueue::Frame().PushFunction(
            [im = image, imView = imageView, device = m_Device]()
            {
                vmaDestroyImage(GetVulkanAllocator(), im.Image, im.Memory);
                device.destroyImageView(imView);
                BeeCoreTrace("Destroyed Vulkan VMA Image successfully! Image count: {}", --VulkanImageCount);
            });
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
        CheckVkResult(m_Device.allocateCommandBuffers(&allocInfo, &commandBuffer));

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        CheckVkResult(commandBuffer.begin(&beginInfo));
        return commandBuffer;
    }

    void VulkanGraphicsDevice::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        CheckVkResult(m_GraphicsQueue.submit(1, &submitInfo, nullptr));
        m_GraphicsQueue.waitIdle();

        m_Device.freeCommandBuffers(m_CommandPool, 1, &commandBuffer);
    }

    void VulkanGraphicsDevice::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
    {
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

        vk::BufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanGraphicsDevice::CopyBufferToImage(
        vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount)
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

        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
        EndSingleTimeCommands(commandBuffer);
    }

    void VulkanGraphicsDevice::CopyImageToImage(
        vk::CommandBuffer cmd, vk::Image source, vk::Image destination, vk::Extent2D srcSize, vk::Extent2D dstSize)
    {
        vk::ImageBlit2 region{};

        region.srcOffsets[1].x = srcSize.width;
        region.srcOffsets[1].y = srcSize.height;
        region.srcOffsets[1].z = 1;

        region.dstOffsets[1].x = dstSize.width;
        region.dstOffsets[1].y = dstSize.height;
        region.dstOffsets[1].z = 1;

        region.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.srcSubresource.layerCount = 1;
        region.srcSubresource.mipLevel = 0;
        region.srcSubresource.baseArrayLayer = 0;

        region.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.dstSubresource.layerCount = 1;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.baseArrayLayer = 0;

        vk::BlitImageInfo2 blitInfo{};
        blitInfo.srcImage = source;
        blitInfo.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal;
        blitInfo.dstImage = destination;
        blitInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal;
        blitInfo.regionCount = 1;
        blitInfo.pRegions = &region;
        blitInfo.filter = vk::Filter::eLinear;

        cmd.blitImage2KHR(blitInfo, g_vkDynamicLoader); // TODO: on mac this segfaults
    }

    void VulkanGraphicsDevice::CopyImageToImage(vk::Image source,
                                                vk::Image destination,
                                                vk::Extent2D srcSize,
                                                vk::Extent2D dstSize)
    {
        auto cmd = BeginSingleTimeCommands();
        CopyImageToImage(cmd, source, destination, srcSize, dstSize);
        EndSingleTimeCommands(cmd);
    }

    void VulkanGraphicsDevice::CreateImageWithInfo(const vk::ImageCreateInfo& imageInfo,
                                                   const vk::ImageViewCreateInfo& imageViewInfo,
                                                   vk::MemoryPropertyFlags memoryProperties,
                                                   const VmaMemoryUsage& memoryUsage,
                                                   VulkanImage& outImage,
                                                   vk::ImageView& outImageView) const
    {
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = memoryUsage;
        allocInfo.requiredFlags = (VkMemoryPropertyFlags)memoryProperties;

        // allocate and create the image
        vk::Result result = (vk::Result)vmaCreateImage(m_DeviceHandle.allocator,
                                                       (VkImageCreateInfo*)&imageInfo,
                                                       &allocInfo,
                                                       (VkImage*)&outImage.Image,
                                                       &outImage.Memory,
                                                       nullptr);
        if (result != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to create image! {}", vk::to_string(result));
        }
        auto copiedImageViewInfo = imageViewInfo;
        copiedImageViewInfo.image = outImage.Image;
        result = m_Device.createImageView(&copiedImageViewInfo, nullptr, &outImageView);

        if (result != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to create image view!");
        }
        outImage.Format = imageInfo.format;
        outImage.Extent = vk::Extent2D{imageInfo.extent.width, imageInfo.extent.height};
        BeeCoreTrace("Created Vulkan VMA Image successfully! Image count: {}", ++VulkanImageCount);
    }

    uint32_t VulkanGraphicsDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void VulkanGraphicsDevice::CreateSwapChainSupportDetails()
    {
        auto& surface = m_DeviceHandle.surface;
        m_SwapChainSupportDetails.capabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(surface);

        uint32_t formatCount;
        CheckVkResult(m_PhysicalDevice.getSurfaceFormatsKHR(surface, &formatCount, nullptr));

        if (formatCount != 0)
        {
            m_SwapChainSupportDetails.formats.resize(formatCount);
            CheckVkResult(
                m_PhysicalDevice.getSurfaceFormatsKHR(surface, &formatCount, m_SwapChainSupportDetails.formats.data()));
        }

        uint32_t presentModeCount;
        CheckVkResult(m_PhysicalDevice.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr));

        if (presentModeCount != 0)
        {
            m_SwapChainSupportDetails.presentModes.resize(presentModeCount);
            CheckVkResult(m_PhysicalDevice.getSurfacePresentModesKHR(
                surface, &presentModeCount, m_SwapChainSupportDetails.presentModes.data()));
        }
    }

    vk::Format VulkanGraphicsDevice::FindSupportedFormat(const std::vector<vk::Format>& candidates,
                                                         vk::ImageTiling tiling,
                                                         vk::FormatFeatureFlags features)
    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties props;
            m_PhysicalDevice.getFormatProperties(format, &props);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    vk::Viewport VulkanGraphicsDevice::CreateVKViewport(uint32_t width, uint32_t height, float depthMin, float depthMax)
    {
        return {0.0f,
                static_cast<float>(
                    height), // y is flipped https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
                static_cast<float>(width),
                -static_cast<float>(height), // y is flipped
                depthMin,
                depthMax};
    }

    void VulkanGraphicsDevice::InitializeVulkanMemoryAllocator(VulkanInstance& instance)
    {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorCreateInfo.physicalDevice = m_PhysicalDevice;
        allocatorCreateInfo.device = m_Device;
        allocatorCreateInfo.instance = instance.GetHandle();
        allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
        allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        vmaCreateAllocator(&allocatorCreateInfo, &m_DeviceHandle.allocator);
    }

    void VulkanGraphicsDevice::WindowResized(uint32_t width, uint32_t height)
    {
        m_Device.waitIdle();
        auto oldSwapChain = m_SwapChain->GetHandle();
        CreateSwapChainSupportDetails();
        if (m_SwapChainSupportDetails.capabilities.currentExtent.width == 0 ||
            m_SwapChainSupportDetails.capabilities.currentExtent.height == 0)
        {
            m_SwapChainRebuildRequested = false;
            return;
        }
        m_SwapChain = CreateScope<VulkanSwapChain>(*this, width, height, oldSwapChain);
        m_SwapChainRebuildRequested = false;
        BeeEnsures(m_SwapChain->GetHandle() != VK_NULL_HANDLE);
        BeeEnsures(m_SwapChain->GetHandle() != oldSwapChain);
    }

    VulkanGraphicsDevice& VulkanGraphicsDevice::GetInstance()
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
        catch (vk::SystemError& e)
        {
            BeeCoreError("Failed to create command pool: {0}", e.what());
        }

        m_CommandPoolAllocateInfo.commandPool = m_CommandPool;
        m_CommandPoolAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
        m_CommandPoolAllocateInfo.commandBufferCount = 1;
    }

    void VulkanGraphicsDevice::CreateDescriptorPool()
    {
        std::vector<vk::DescriptorPoolSize> poolSizes = {
            {vk::DescriptorType::eUniformBuffer, 1000},
            {vk::DescriptorType::eSampler, 1000},
            {vk::DescriptorType::eSampledImage, 1000},
        };
        vk::DescriptorPoolCreateInfo poolInfo = {};
        poolInfo.flags = vk::DescriptorPoolCreateFlags() | vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1000;
        CheckVkResult(m_Device.createDescriptorPool(&poolInfo, nullptr, &m_DescriptorPool));
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
} // namespace BeeEngine::Internal
#endif