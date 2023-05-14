//
// Created by alexl on 27.01.2023.
//

#pragma once
#include "Core/Logging/Log.h"
#include "RendererAPI.h"
#include "Core/Color4.h"

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
            s_RendererAPI = RendererAPI::Create();
            s_RendererAPI->Init();
        }
        static void SetClearColor(const Color4& color)
        {
            s_RendererAPI->SetClearColor(color);
        }
        static void Clear()
        {
            s_RendererAPI->Clear();
        }
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            s_RendererAPI->SetViewport(x, y, width, height);
        }
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
        {
            vertexArray->Bind();
            s_RendererAPI->DrawIndexed(vertexArray, indexCount);
        }

    private:
        static RenderAPI s_Api;
        static Ref<RendererAPI> s_RendererAPI;
    };
}
