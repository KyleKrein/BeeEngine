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
        virtual void Init() override;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray);
    };
}

