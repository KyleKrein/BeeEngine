//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#pragma once

#include "Renderer/Surface.h"
#include "vulkan/vulkan.hpp"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine::Internal
{
    class VulkanSurface: public Surface
    {
    public:
        VulkanSurface(WindowHandlerAPI windowApi, vk::Instance &instance);
        ~VulkanSurface() override;

        [[nodiscard]] vk::SurfaceKHR &GetHandle() { return m_Surface; }

    private:
        vk::Instance& m_Instance;
        vk::SurfaceKHR m_Surface;
        WindowHandlerAPI m_WindowApi;
    };
}
#endif