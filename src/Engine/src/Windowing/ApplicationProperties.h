//
// Created by Александр Лебедев on 23.01.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "Renderer/RenderAPI.h"
#include "VSync.h"

namespace BeeEngine
{
    struct ApplicationProperties
    {
        enum class StartMode
        {
            UI,
            Headless // TODO: Implement headless mode
        };
        uint16_t WindowWidth = 1280; // Platform dependant. Does nothing in headless mode
        uint16_t WindowHeight = 720; // Platform dependant. Does nothing in headless mode
        String Title = "BeeEngine App";
        VSync Vsync = VSync::On; // Platform dependant. Does nothing in headless mode
        RenderAPI PreferredRenderAPI = GetPrefferedRenderAPI();
        int32_t WindowXPosition = 100; // Platform dependant. Does nothing in headless mode or on mobile
        int32_t WindowYPosition = 100; // Platform dependant. Does nothing in headless mode or on mobile
        bool IsMaximized = false;      // Platform dependant. Does nothing in headless mode
        const StartMode Mode = StartMode::UI;
    };
} // namespace BeeEngine
