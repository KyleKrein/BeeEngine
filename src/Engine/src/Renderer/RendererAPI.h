//
// Created by alexl on 27.01.2023.
//

#pragma once

#include <glm/glm.hpp>
#include "Core/TypeDefines.h"
#include "VertexArray.h"
#include "Core/Color4.h"

namespace BeeEngine
{
    class RendererAPI {
    public:
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;
        virtual void SetClearColor(const Color4& color) = 0;
        virtual void Clear() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;
        static Ref<RendererAPI> Create();
    };
}