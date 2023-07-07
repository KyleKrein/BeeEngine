//
// Created by Александр Лебедев on 26.06.2023.
//

#pragma once

#include "vk_mem_alloc.h"

namespace BeeEngine::Internal
{
    struct VulkanImage
    {
        VkImage Image;
        VmaAllocation Memory;

        void Destroy();
    };
}