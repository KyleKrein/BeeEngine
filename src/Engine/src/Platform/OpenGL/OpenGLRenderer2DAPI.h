//
// Created by alexl on 14.05.2023.
//

#pragma once

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
        glm::vec4 RectVertexPositions[4];

        Renderer2DData(Ref<Renderer2D::Statistics> statistics)
        : Stats(statistics), RectVerticesBuffer(), TextureSlots() //TODO: get from GPU
        {
            //TextureSlots.reserve(MaxTextureSlots);
        }
        Renderer2DData()
        : Stats(nullptr), RectVerticesBuffer(), TextureSlots()
        {
            //TextureSlots.reserve(MaxTextureSlots);
        }

        Ref<Renderer2D::Statistics> Stats;
    };


    class OpenGLRenderer2DAPI: public Renderer2DAPI
    {
    public:
        virtual void Init() override;
        virtual void DrawRectangle(float x, float y, float z, float width, float height, const Color4 &color, float rotation) override;
        virtual void DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D>& texture, float rotation, const Color4& color, float textureMultiplier) override;
        virtual ~OpenGLRenderer2DAPI() override = default;

        virtual void BeginScene() override;
        virtual void EndScene() override;
        virtual void Flush() override;

        virtual void SetCameraTransform(const glm::mat4& transform) override;

        private:
        static glm::vec2 s_textureCoords[4];
        Renderer2DData m_Data;
        static const int RectVertexCount = 4;

        void FlushAndReset();
    };
}
