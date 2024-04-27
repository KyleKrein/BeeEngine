//
// Created by Aleksandr on 02.03.2024.
//

#include "Hardware.h"

#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "Renderer/Renderer.h"
#include <thread>

#if defined(BEE_COMPILE_SDL)
#include <SDL3/SDL.h>
#endif

namespace BeeEngine
{
    uint32_t Hardware::GetNumberOfCores()
    {
        return std::thread::hardware_concurrency();
    }

    bool Hardware::HasRayTracingSupport()
    {
        switch (Renderer::GetAPI()) {
            case WebGPU:
                return false;
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return Internal::VulkanGraphicsDevice::GetInstance().HasRayTracingSupport();
#endif
            default:
                return false;
        }
    }

    uint32_t BeeEngine::Hardware::GetSystemRAM()
    {
        switch (WindowHandler::GetAPI())
        {
#if defined(BEE_COMPILE_SDL)
            case WindowHandlerAPI::SDL:
                return SDL_GetSystemRAM();
#endif
            case WindowHandlerAPI::WinAPI:
#if defined(WINDOWS)

#endif
                break;
        }
        BeeCoreError("GetSystemRAM not implemented for this platform");
        return 0;
    }

    uint64_t Hardware::GetGPUVRAM()
    {
        return WindowHandler::GetInstance()->GetGraphicsDevice().GetVRAM();
    }

    BeeEngine::Hardware::SystemTheme BeeEngine::Hardware::GetSystemTheme()
    {
        using enum BeeEngine::Hardware::SystemTheme;

        switch (WindowHandler::GetAPI())
        {
#if defined(BEE_COMPILE_SDL)
            case WindowHandlerAPI::SDL:
            {
                auto theme = SDL_GetSystemTheme();
                switch (theme)
                {
                    case SDL_SYSTEM_THEME_LIGHT:
                        return SystemTheme::Light;
                    case SDL_SYSTEM_THEME_DARK:
                        return SystemTheme::Dark;
                    default:
                        return SystemTheme::Unknown;
                }
            }
#endif
            case WindowHandlerAPI::WinAPI:
#if defined(WINDOWS)

#endif
                break;
        }
        BeeCoreError("GetSystemTheme not implemented for this platform");
        return SystemTheme::Unknown;
    }

}
