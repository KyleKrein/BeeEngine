//
// Created by Александр Лебедев on 23.01.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "VSync.h"
#include "Renderer/RenderAPI.h"

namespace BeeEngine{
    struct WindowProperties
    {
        uint16_t Width = 1280;
        uint16_t Height = 720;
        const char *Title = "BeeEngine App";
        VSync Vsync = VSync::On;
        RenderAPI PreferredRenderAPI = GetPrefferedRenderAPI();
    };
}
