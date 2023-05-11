//
// Created by alexl on 27.01.2023.
//

#pragma once

#include <glm/glm.hpp>
#include "Core/TypeDefines.h"
#include "Renderer.h"

namespace BeeEngine
{
    class RendererAPI {
    public:
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

        inline static RenderAPI GetAPI() { return Renderer::GetAPI(); }
        static Ref<RendererAPI> Create();
    };
}