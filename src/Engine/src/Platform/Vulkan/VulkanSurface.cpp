//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#include "VulkanSurface.h"
#include "SDL3/SDL_vulkan.h"


namespace BeeEngine::Internal
{

    VulkanSurface::VulkanSurface(WindowHandlerAPI windowApi, vk::Instance &instance)
    : m_Instance(instance)
    {
        m_WindowApi = windowApi;
        VkSurfaceKHR cSurface;
#if defined(DESKTOP_PLATFORM) && defined(BEE_COMPILE_GLFW)
        if(windowApi == WindowHandlerAPI::GLFW)
        {

            auto glfwWindow = (GLFWwindow*)WindowHandler::GetInstance()->GetWindow();
            auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &cSurface);
            if(result != VK_SUCCESS)
            {
                BeeCoreFatalError("Failed to create Vulkan surface!");
            }
        }
        else
        {
#endif
            auto sdlWindow = (SDL_Window*)WindowHandler::GetInstance()->GetWindow();
            auto result = SDL_Vulkan_CreateSurface(sdlWindow, instance, &cSurface);
            if(result != SDL_TRUE)
            {
                BeeCoreFatalError("Failed to create Vulkan surface!");
            }
#if defined(DESKTOP_PLATFORM) && defined(BEE_COMPILE_GLFW)
        }
#endif
        m_Surface = vk::SurfaceKHR(cSurface);
    }

    VulkanSurface::~VulkanSurface()
    {
        m_Instance.destroySurfaceKHR(m_Surface);
    }
}
#endif