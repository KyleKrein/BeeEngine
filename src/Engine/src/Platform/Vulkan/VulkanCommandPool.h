//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/CommandPool.h"
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanCommandPool: public CommandPool
    {
    public:

    private:
        VkCommandPool m_CommandPool;
    };
}
