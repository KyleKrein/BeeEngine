//
// Created by alexl on 14.05.2023.
//

#include "OpenGLRenderer2DAPI.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Renderer.h"
#include "glm.hpp"
#include "ext/matrix_transform.hpp"
#include "Renderer/ShaderLibrary.h"


namespace BeeEngine::Internal
{

    void OpenGLRenderer2DAPI::Init()
    {
        BEE_PROFILE_FUNCTION();
        m_Data = Renderer2DData(Renderer2D::GetStatistics());
        m_Data.RectVertexBuffer = GraphicsBuffer::CreateVertexBuffer(BeeEngine::Internal::Renderer2DData::MaxVertices * sizeof(RectVertex));
        m_Data.RectVertexBuffer->SetLayout({
                                                   {ShaderDataType::Float3, "a_Position"},
                                                   {ShaderDataType::Float4, "a_Color"},
                                                   {ShaderDataType::Float2, "a_TexCoord"},
                                                   {ShaderDataType::Float, "a_TextureIndex"},
                                                   {ShaderDataType::Float, "a_TilingFactor"},
                                                   {ShaderDataType::Int, "a_EntityID"}
                                           });

        uint32_t* rectangleIndices = new uint32_t[BeeEngine::Internal::Renderer2DData::MaxIndices];
        uint32_t offset = 0;
        for (int i = 0; i < BeeEngine::Internal::Renderer2DData::MaxIndices; i += 6)
        {
            rectangleIndices[i + 0] = offset + 0;
            rectangleIndices[i + 1] = offset + 1;
            rectangleIndices[i + 2] = offset + 2;

            rectangleIndices[i + 3] = offset + 2;
            rectangleIndices[i + 4] = offset + 3;
            rectangleIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<GraphicsBuffer> indexBuffer = GraphicsBuffer::CreateIndexBuffer({rectangleIndices, BeeEngine::Internal::Renderer2DData::MaxIndices});
        m_Data.VertexArray = VertexArray::Create(m_Data.RectVertexBuffer, indexBuffer);
        delete[] rectangleIndices;

        m_Data.BlankTexture = Texture2D::Create(1, 1);
        uint32_t blankTexturePixel = 0xffffffff;
        m_Data.BlankTexture->SetData({(std::byte*)&blankTexturePixel, sizeof(uint32_t)});

        int samplers[BeeEngine::Internal::Renderer2DData::MaxTextureSlots];
        for (int i = 0; i < BeeEngine::Internal::Renderer2DData::MaxTextureSlots; i++)
            samplers[i] = i;

        ShaderLibrary::GetInstance().LoadStandartShaders();
        m_Data.TextureShader = ShaderLibrary::GetInstance().Get("Standart2DShader");
        m_Data.TextureShader->Bind();
        m_Data.TextureShader->SetIntArray("u_Textures", {samplers, BeeEngine::Internal::Renderer2DData::MaxTextureSlots});

        m_Data.TextureSlots[0] = m_Data.BlankTexture;
    }

    void OpenGLRenderer2DAPI::BeginScene()
    {
        BEE_PROFILE_FUNCTION();
        m_Data.CurrentVertex = m_Data.RectVerticesBuffer;
        m_Data.RectIndexCount = 0;

        m_Data.TextureSlotIndex = 1;
        //memset(m_Data.TextureSlots.data() + 1, 0, (m_Data.MaxTextureSlots - 1)  * sizeof(Ref<Texture2D>));
        for(int i = 1; i < BeeEngine::Internal::Renderer2DData::MaxTextureSlots; i++)
            m_Data.TextureSlots[i] = nullptr;
    }

    void OpenGLRenderer2DAPI::EndScene()
    {
        BEE_PROFILE_FUNCTION();
        Flush();
    }

    void OpenGLRenderer2DAPI::Flush()
    {
        BEE_PROFILE_FUNCTION();
        if (m_Data.RectIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.CurrentVertex - (uint8_t*)m_Data.RectVerticesBuffer);
            m_Data.RectVertexBuffer->SetData({(std::byte*)m_Data.RectVerticesBuffer, dataSize});

            // Bind textures
            for (uint32_t i = 0; i < m_Data.TextureSlotIndex; i++)
                m_Data.TextureSlots[i]->Bind(i);

            m_Data.TextureShader->Bind();
            Renderer::DrawIndexed(m_Data.VertexArray, m_Data.RectIndexCount);
            m_Data.Stats->DrawCalls++;
        }
    }

    void OpenGLRenderer2DAPI::DrawRectangle(const glm::mat4& transform, const Color4 &color)
    {
        DrawRectangleWithID(transform, color, -1);
    }

    void OpenGLRenderer2DAPI::FlushAndReset()
    {
        BEE_PROFILE_FUNCTION();
        if(m_Data.RectIndexCount >= BeeEngine::Internal::Renderer2DData::MaxIndices)
        {
            EndScene();
            BeginScene();
        }
    }

    void OpenGLRenderer2DAPI::DrawImage(const glm::mat4& transform, const Ref<Texture2D>& texture, const Color4& color, float textureMultiplier)
    {
        DrawImageWithID(transform, texture, color, textureMultiplier, -1);
    }

    void OpenGLRenderer2DAPI::SetCameraTransform(const glm::mat4 &transform)
    {
        BEE_PROFILE_FUNCTION();
        m_Data.TextureShader->Bind();
        m_Data.TextureShader->SetMat4("u_ViewProjection", transform);
    }

    void OpenGLRenderer2DAPI::DrawRectangleWithID(const glm::mat4 &transform, const Color4 &color, int entityID)
    {
        BEE_PROFILE_FUNCTION();
        static const float blankTextureIndex = 0.0f;
        static const float TilingFactor = 1.0f;

        FlushAndReset();

        for (int i = 0; i < RectVertexCount; i++)
        {
            m_Data.CurrentVertex->Position = transform * BeeEngine::Internal::Renderer2DData::RectVertexPositions[i];
            m_Data.CurrentVertex->Color = color;
            m_Data.CurrentVertex->TextureCoords = s_textureCoords[i];
            m_Data.CurrentVertex->TextureIndex = blankTextureIndex;
            m_Data.CurrentVertex->TilingFactor = TilingFactor;
            m_Data.CurrentVertex->EntityID = entityID;
            m_Data.CurrentVertex++;
        }

        m_Data.RectIndexCount += 6;
    }

    void OpenGLRenderer2DAPI::DrawImageWithID(const glm::mat4 &transform, const Ref<Texture2D> &texture, const Color4 &color,
                                         float textureMultiplier, int entityID)
    {
        BEE_PROFILE_FUNCTION();
        FlushAndReset();

        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < m_Data.TextureSlotIndex; i++)
        {
            if (*m_Data.TextureSlots[i].get() == *texture.get()) // TODO: Check for the bug
            {
                textureIndex = (float)i;
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            if (m_Data.TextureSlotIndex >= BeeEngine::Internal::Renderer2DData::MaxTextureSlots)
                FlushAndReset();
            textureIndex = (float)m_Data.TextureSlotIndex;
            m_Data.TextureSlots[m_Data.TextureSlotIndex] = texture;
            m_Data.TextureSlotIndex++;
        }

        for (int i = 0; i < RectVertexCount; i++)
        {
            m_Data.CurrentVertex->Position = transform * BeeEngine::Internal::Renderer2DData::RectVertexPositions[i];
            m_Data.CurrentVertex->Color = color;
            m_Data.CurrentVertex->TextureCoords = s_textureCoords[i];
            m_Data.CurrentVertex->TextureIndex = textureIndex;
            m_Data.CurrentVertex->TilingFactor = textureMultiplier;
            m_Data.CurrentVertex->EntityID = entityID;
            m_Data.CurrentVertex++;
        }

        m_Data.RectIndexCount += 6;

        m_Data.Stats->QuadCount++;
        m_Data.Stats->SpriteCount++;
    }
}
