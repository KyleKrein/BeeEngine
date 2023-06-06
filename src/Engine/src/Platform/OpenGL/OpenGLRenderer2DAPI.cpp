//
// Created by alexl on 14.05.2023.
//

#include "OpenGLRenderer2DAPI.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Renderer.h"
#include "glm.hpp"
#include "ext/matrix_transform.hpp"


namespace BeeEngine::Internal
{

    void OpenGLRenderer2DAPI::Init()
    {
        BEE_PROFILE_FUNCTION();
        m_Data = Renderer2DData(Renderer2D::GetStatistics());

        static const String ShaderName = String("StandartBeeEngine2DShader");
        static const String VertexShader = "#version 450 core\n"
                                           "\t\t\t\n"
                                           "layout(location = 0) in vec3 a_Position;\n"
                                           "layout(location = 1) in vec4 a_Color;\n"
                                           "layout(location = 2) in vec2 a_TexCoord;\n"
                                           "layout(location = 3) in float a_TextureIndex;\n"
                                           "layout(location = 4) in float a_TilingFactor;\n"
                                           "\n"
                                           "uniform mat4 u_ViewProjection;\n"
                                           "\n"
                                           "out vec2 v_TexCoord;\n"
                                           "out vec4 v_Color;\n"
                                           "out flat float v_TextureIndex;\n"
                                           "out float v_TilingFactor;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    v_Color = a_Color;\n"
                                           "\tv_TexCoord = a_TexCoord;\n"
                                           "\tv_TextureIndex = a_TextureIndex;\n"
                                           "\tv_TilingFactor = a_TilingFactor;\n"
                                           "\tgl_Position =  u_ViewProjection * vec4(a_Position, 1.0);\t\n"
                                           "}";
        static const String FragmentShader = "#version 450 core\n"
                                             "\t\t\t\n"
                                             "layout(location = 0) out vec4 color;\n"
                                             "in vec2 v_TexCoord;\n"
                                             "in vec4 v_Color;\n"
                                             "in flat float v_TextureIndex;\n"
                                             "in float v_TilingFactor;\n"
                                             "\n"
                                             "uniform sampler2D u_Textures[16];\n"
                                             "void main()\n"
                                             "{\n"
                                             "\tcolor = texture(u_Textures[int(v_TextureIndex)], v_TexCoord * v_TilingFactor) * v_Color;\n"
                                             "}";

        m_Data.TextureShader = Shader::Create(ShaderName, VertexShader, FragmentShader);

        int samplers[BeeEngine::Internal::Renderer2DData::MaxTextureSlots];
        for (int i = 0; i < BeeEngine::Internal::Renderer2DData::MaxTextureSlots; i++)
            samplers[i] = i;

        m_Data.TextureShader->Bind();
        m_Data.TextureShader->SetIntArray("u_Textures", {samplers, BeeEngine::Internal::Renderer2DData::MaxTextureSlots});

        m_Data.RectVertexBuffer = GraphicsBuffer::CreateVertexBuffer(BeeEngine::Internal::Renderer2DData::MaxVertices * sizeof(RectVertex));
        m_Data.RectVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TexCoord"},
            {ShaderDataType::Float, "a_TextureIndex"},
            {ShaderDataType::Float, "a_TilingFactor"}
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
        //memset(m_Data.TextureSlots.data(), 0, m_Data.MaxTextureSlots * sizeof(Ref<Texture2D>));
        for(int i = 1; i < BeeEngine::Internal::Renderer2DData::MaxTextureSlots; i++)
            m_Data.TextureSlots[i] = nullptr;
        m_Data.TextureSlots[0] = m_Data.BlankTexture;
        //m_Data.TextureSlots.push_back(m_Data.BlankTexture);
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
            m_Data.CurrentVertex++;
        }

        m_Data.RectIndexCount += 6;
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

    void
    OpenGLRenderer2DAPI::DrawImage(const glm::mat4& transform, const Ref<Texture2D>& texture, const Color4& color, float textureMultiplier)
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
            m_Data.CurrentVertex++;
        }

        m_Data.RectIndexCount += 6;

        m_Data.Stats->QuadCount++;
        m_Data.Stats->SpriteCount++;
    }

    void OpenGLRenderer2DAPI::SetCameraTransform(const glm::mat4 &transform)
    {
        BEE_PROFILE_FUNCTION();
        m_Data.TextureShader->Bind();
        m_Data.TextureShader->SetMat4("u_ViewProjection", transform);
    }
}
