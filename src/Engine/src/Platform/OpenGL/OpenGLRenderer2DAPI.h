//
// Created by alexl on 14.05.2023.
//

#pragma once

#include <utility>

#include "Core/TypeDefines.h"
#include "Renderer/Renderer2DAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Renderer2D.h"

namespace BeeEngine::Internal
{
    struct RectVertex
    {
        glm::vec3 Position;
        Color4 Color;
        glm::vec2 TextureCoords;
        float TextureIndex;
        float TilingFactor;

        //Editor only
        int EntityID;
    };

    struct Renderer2DData
    {
        static const int MaxRectangles = 10000;
        static const int MaxVertices = MaxRectangles * 4;
        static const int MaxIndices = MaxRectangles * 6;
        static const int MaxTextureSlots = 16;

        Ref<Shader> TextureShader;
        Ref<VertexArray> VertexArray;
        Ref<Texture2D> BlankTexture;

        Ref<GraphicsBuffer> RectVertexBuffer;
        int RectIndexCount = 0;
        RectVertex RectVerticesBuffer[MaxVertices];
        RectVertex* CurrentVertex = RectVerticesBuffer;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        int TextureSlotIndex = 1; //0 = blank texture
        constexpr static glm::vec4 RectVertexPositions[4]
                {
                        {-0.5f, -0.5f, 0.0f, 1.0f},
                        {0.5f, -0.5f, 0.0f, 1.0f},
                        {0.5f, 0.5f, 0.0f, 1.0f},
                        {-0.5f, 0.5f, 0.0f, 1.0f}
                };

        explicit Renderer2DData(Ref<Renderer2D::Statistics> statistics)
        : RectVerticesBuffer(), TextureSlots(), Stats(std::move(statistics)) //TODO: get from GPU
        {
            //TextureSlots.reserve(MaxTextureSlots);
        }
        Renderer2DData()
        : RectVerticesBuffer(), TextureSlots(), Stats(nullptr)
        {
            //TextureSlots.reserve(MaxTextureSlots);
        }

        Ref<Renderer2D::Statistics> Stats;
    };


    class OpenGLRenderer2DAPI: public Renderer2DAPI
    {
    public:
        virtual void Init() override;
        virtual void DrawRectangle(const glm::mat4& transform, const Color4 &color) override;
        virtual void DrawImage(const glm::mat4& transform, const Ref<Texture2D>& texture, const Color4& color, float textureMultiplier) override;
        virtual void DrawRectangleWithID(const glm::mat4& transform, const Color4 &color, int entityID) override;
        virtual void DrawImageWithID(const glm::mat4& transform, const Ref<Texture2D>& texture, const Color4& color, float textureMultiplier, int entityID) override;
        virtual ~OpenGLRenderer2DAPI() override = default;

        virtual void BeginScene() override;
        virtual void EndScene() override;
        virtual void Flush() override;

        virtual void SetCameraTransform(const glm::mat4& transform) override;

        private:
        constexpr static glm::vec2 s_textureCoords[4]
        {
                {0.0f, 0.0f},
                {1.0f, 0.0f},
                {1.0f, 1.0f},
                {0.0f, 1.0f}
        };
        Renderer2DData m_Data;
        static const int RectVertexCount = 4;

        void FlushAndReset();
    };
}
