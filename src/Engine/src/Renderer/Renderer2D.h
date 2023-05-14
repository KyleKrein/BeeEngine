//
// Created by alexl on 14.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "Core/Cameras/Camera.h"
#include "glm/glm.hpp"
#include "Renderer/RectangleProperties.h"
#include "Texture.h"
#include "Renderer2DAPI.h"

namespace BeeEngine
{
    class Renderer2D
    {

    public:
        static void BeginScene(const Camera& camera);
        static void EndScene();
        static void Init(Renderer2DAPI* api);
        static void DrawRectangle(const RectangleProperties& properties);
        static void DrawRectangle(const glm::vec2& position, const glm::vec2& size, const Color4& color);
        static void DrawRectangle(float x, float y, float z, float width, float height, const Color4& color);
        static void DrawRectangle(float x, float y, float z, float width, float height, const Color4& color, float rotation);

        static void DrawImage(float x, float y, float width, float height, const Ref<Texture2D>& texture);
        static void DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D>& texture);
        static void DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D>& texture, float rotation);
        static void DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D>& texture, float rotation, const Color4& color);
        static void DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D>& texture, float rotation, const Color4& color, float textureMultiplier);

        static void ResetStatistics();


        static void Shutdown();

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            uint32_t SpriteCount = 0;

            uint32_t GetTotalVertexCount() { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() { return QuadCount * 6; }
        };

        static Ref<Statistics> GetStatistics() { return m_Statistics; }

    private:
        static Renderer2DAPI* m_API;
        static Ref<Statistics> m_Statistics;
    };
}
