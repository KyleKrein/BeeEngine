//
// Created by Александр Лебедев on 07.05.2023.
//

#pragma once
#include "Renderer/RendererAPI.h"

namespace BeeEngine
{
    class OpenGLRendererAPI: public RendererAPI
    {
    public:
        void Init() override;
        void SetClearColor(const Color4& color) override;
        void Clear() override;

        void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;

        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual Color4 ReadPixel(uint32_t x, uint32_t y) override;
    };
}

