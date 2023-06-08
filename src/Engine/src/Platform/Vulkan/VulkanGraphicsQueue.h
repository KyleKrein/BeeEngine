//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/GraphicsQueue.h"
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanGraphicsQueue: public GraphicsQueue
    {
    public:

    private:
        VkQueue m_Queue;
    };
}
