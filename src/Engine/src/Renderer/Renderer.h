//
// Created by alexl on 27.01.2023.
//

#pragma once
#include "RendererAPI.h"
#include "Core/Logging/Log.h"

namespace BeeEngine
{
    class Renderer
    {
    public:
        static RendererAPI GetAPI()
        {
            return s_Api;
        }
        static void SetAPI(const RendererAPI& api)
        {
            BeeCoreAssert(s_Api, "Can't change Renderer API after initialization!");
            s_Api = api;
        }
    private:
        static RendererAPI s_Api;
    };
}
