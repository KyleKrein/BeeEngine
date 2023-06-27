//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once

#include "Renderer/Renderer2DAPI.h"

namespace BeeEngine::Internal
{
    class VulkanRenderer2DAPI: public Renderer2DAPI
    {
    public:
        void Init() override;
        void DrawRectangle(const glm::mat4& transform, const Color4 &color) override
        {
            DrawRectangleWithID(transform, color, -1);
        }
        void DrawImage(const glm::mat4& transform, const Ref<Texture2D>& texture, const Color4& color = Color4::White, float textureMultiplier = 1) override
        {
            DrawImageWithID(transform, texture, color, textureMultiplier, -1);
        }

        void DrawRectangleWithID(const glm::mat4& transform, const Color4 &color, int entityID) override;
        void DrawImageWithID(const glm::mat4& transform, const Ref<Texture2D>& texture, const Color4& color, float textureMultiplier, int entityID) override;

        void SetCameraTransform(const glm::mat4& transform) override;
        void BeginScene() override;
        void EndScene() override;
        void Flush() override;

        ~VulkanRenderer2DAPI() override = default;

    private:

    };
}