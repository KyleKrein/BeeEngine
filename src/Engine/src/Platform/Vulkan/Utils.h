//
// Created by Александр Лебедев on 26.06.2023.
//

#pragma once
#include "Core/Logging/Log.h"
#if defined(BEE_COMPILE_VULKAN)
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    extern PFN_vkCmdTraceRaysKHR CmdTraceRaysKHR;
    extern PFN_vkDestroyAccelerationStructureKHR DestroyAccelerationStructureKHR;
    extern PFN_vkGetRayTracingShaderGroupHandlesKHR GetRayTracingShaderGroupHandlesKHR;
    extern PFN_vkCmdWriteAccelerationStructuresPropertiesKHR
        CmdWriteAccelerationStructuresPropertiesKHR;
    extern PFN_vkCreateAccelerationStructureKHR CreateAccelerationStructureKHR;
    extern PFN_vkCmdBuildAccelerationStructuresKHR CmdBuildAccelerationStructuresKHR;
    extern PFN_vkCmdCopyAccelerationStructureKHR CmdCopyAccelerationStructureKHR;
    extern PFN_vkCreateRayTracingPipelinesKHR CreateRayTracingPipelinesKHR;
    extern PFN_vkGetAccelerationStructureDeviceAddressKHR GetAccelerationStructureDeviceAddressKHR;
    extern PFN_vkGetAccelerationStructureBuildSizesKHR GetAccelerationStructureBuildSizesKHR;

    inline vk::DispatchLoaderDynamic g_vkDynamicLoader;

    inline void CheckVkResult(vk::Result result)
    {
        if (result != vk::Result::eSuccess)
        {
            BeeCoreError("Vulkan error: " + vk::to_string(result));
        }
    }

    struct VulkanInitializer
    {
        static vk::ImageCreateInfo ImageCreateInfo(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent3D extent);
        static vk::ImageViewCreateInfo ImageViewCreateInfo(vk::Format format, vk::Image image, vk::ImageAspectFlags aspectFlags);
        static vk::PipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, vk::CompareOp compareOp);
    };
}
#endif