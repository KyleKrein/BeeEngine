//
// Created by alexl on 09.06.2023.
//

#include "VulkanSurface.h"


namespace BeeEngine::Internal
{

    VulkanSurface::VulkanSurface(WindowHandlerAPI windowApi, vk::Instance &instance)
    : m_Instance(instance)
    {
        if(windowApi == WindowHandlerAPI::SDL)
        {
            BeeCoreFatalError("SDL WindowHandlerAPI not implemented yet!");
        }

        m_WindowApi = windowApi;
        auto glfwWindow = (GLFWwindow*)WindowHandler::GetInstance()->GetWindow();
        VkSurfaceKHR cSurface;
        auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &cSurface);
        if(result != VK_SUCCESS)
        {
            BeeCoreFatalError("Failed to create Vulkan surface!");
        }
        m_Surface = vk::SurfaceKHR(cSurface);
    }

    VulkanSurface::~VulkanSurface()
    {
        m_Instance.destroySurfaceKHR(m_Surface);
    }
}