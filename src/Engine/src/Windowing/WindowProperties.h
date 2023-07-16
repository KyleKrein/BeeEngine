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
        uint16_t Width;
        uint16_t Height;
        const char *Title;
        VSync Vsync;
        const RenderAPI PreferredRenderAPI = RenderAPI::WebGPU;
    };
}
