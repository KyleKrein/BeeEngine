//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "Core/Color4.h"
#include "Texture.h"
#include "glm.hpp"

namespace BeeEngine
{
    class Renderer2DAPI
    {
    public:
        virtual void Init() = 0;
        virtual void DrawRectangle(float x, float y, float z, float width, float height, const Color4 &color, float rotation = 0) = 0;
        virtual void DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D>& texture, float rotation = 0, const Color4& color = Color4::White, float textureMultiplier = 1) = 0;

        virtual void SetCameraTransform(const glm::mat4& transform) = 0;
        virtual void BeginScene() = 0;
        virtual void EndScene() = 0;
        virtual void Flush() = 0;

        virtual ~Renderer2DAPI() {};
    };
}
