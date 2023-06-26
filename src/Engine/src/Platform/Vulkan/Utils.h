//
// Created by Александр Лебедев on 26.06.2023.
//

#pragma once

#include "vk_mem_alloc.h"

namespace BeeEngine::Internal
{
    struct VulkanInitializer
    {
        static VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
        static VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
        static VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);
    };
}
