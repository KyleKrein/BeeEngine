//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/Surface.h"
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanSurface: public Surface
    {
    public:

    private:
        VkSurfaceKHR m_Surface;
    };
}
