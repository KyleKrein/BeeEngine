//
// Created by alexl on 27.01.2023.
//

#pragma once
#include "RenderAPI.h"
#include "Core/Logging/Log.h"
#include "RendererAPI.h"
#include "Core/Color4.h"
#include "Debug/Instrumentor.h"
#include "Core/CodeSafety/Expects.h"

namespace BeeEngine
{
    class Renderer
    {
    public:
        static RenderAPI GetAPI()
        {
            return s_Api;
        }
        static void SetAPI(const RenderAPI& api)
        {
            BEE_PROFILE_FUNCTION();
            BeeCoreAssert(s_Api == RenderAPI::NotAvailable, "Can't change Renderer API after initialization!");
            s_Api = api;
            //s_RendererAPI = RendererAPI::Create();
            //s_RendererAPI->Init();
        }
        static void SetClearColor(const Color4& color)
        {
            BEE_PROFILE_FUNCTION();
            s_ClearColor = color;
            s_RendererAPI->SetClearColor(color);
        }
        static const Color4& GetClearColor()
        {
            return s_ClearColor;
        }
        static void Clear()
        {
            BEE_PROFILE_FUNCTION();
            s_RendererAPI->Clear();
        }
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(width > 0 && height > 0);
            s_RendererAPI->SetViewport(x, y, width, height);
        }
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
        {
            BEE_PROFILE_FUNCTION();
            vertexArray->Bind();
            s_RendererAPI->DrawIndexed(vertexArray, indexCount);
        }
        static Color4 ReadPixel(uint32_t x, uint32_t y)
        {
            BEE_PROFILE_FUNCTION();
            return s_RendererAPI->ReadPixel(x, y);
        }

    private:
        static RenderAPI s_Api;
        static Ref<RendererAPI> s_RendererAPI;
        static Color4 s_ClearColor;
    };
}
