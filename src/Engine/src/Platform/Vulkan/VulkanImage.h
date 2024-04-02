//
// Created by Александр Лебедев on 26.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_VULKAN)
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.hpp>

namespace BeeEngine::Internal
{
    struct VulkanImage
    {
        vk::Image Image = nullptr;
        VmaAllocation Memory = nullptr;
        vk::Format Format;
        vk::Extent2D Extent;
        operator bool () const
        {
            return Image && Memory;
        }
    };
}
#endif