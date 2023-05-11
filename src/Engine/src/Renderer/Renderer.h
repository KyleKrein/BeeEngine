//
// Created by alexl on 27.01.2023.
//

#pragma once
#include "Core/Logging/Log.h"

namespace BeeEngine
{
    enum RenderAPI
    {
        NotAvailable = 0,
        OpenGL = 1,
        Metal = 2,
        DirectX = 3,
        Vulkan = 4
    };
    class Renderer
    {
    public:
        static const RenderAPI GetAPI()
        {
            return s_Api;
        }
        static void SetAPI(const RenderAPI& api)
        {
            BeeCoreAssert(s_Api == RenderAPI::NotAvailable, "Can't change Renderer API after initialization!");
            s_Api = api;
        }
    private:
        static RenderAPI s_Api;
    };
}
