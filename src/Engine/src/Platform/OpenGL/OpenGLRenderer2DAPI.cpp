//
// Created by alexl on 14.05.2023.
//

#include "OpenGLRenderer2DAPI.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Renderer.h"
#include "glm.hpp"
#include "ext/matrix_transform.hpp"


namespace BeeEngine
{
    glm::vec2 OpenGLRenderer2DAPI::s_textureCoords[4] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
    };

    void OpenGLRenderer2DAPI::Init()
    {
        m_Data = Renderer2DData(Renderer2D::GetStatistics());

        static const Ref<String> ShaderName = CreateRef<String>("StandartBeeEngine2DShader");
        static const String VertexShader = "#version 330 core\n"
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
                                           "out float v_TextureIndex;\n"
                                           "out float v_TilingFactor;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    v_Color = a_Color;\n"
                                           "\tv_TexCoord = a_TexCoord;\n"
                                           "\tv_TextureIndex = a_TextureIndex;\n"
                                           "\tv_TilingFactor = a_TilingFactor;\n"
                                           "\tgl_Position =  u_ViewProjection * vec4(a_Position, 1.0);\t\n"
                                           "}";
        static const String FragmentShader = "#version 330 core\n"
                                             "\t\t\t\n"
                                             "layout(location = 0) out vec4 color;\n"
                                             "in vec2 v_TexCoord;\n"
                                             "in vec4 v_Color;\n"
                                             "in float v_TextureIndex;\n"
                                             "in float v_TilingFactor;\n"
                                             "\n"
                                             "uniform sampler2D u_Textures[16];\n"
                                             "void main()\n"
                                             "{\n"
                                             "\tcolor = texture(u_Textures[int(v_TextureIndex)], v_TexCoord * v_TilingFactor) * v_Color;\n"
                                             "}";

        m_Data.TextureShader = Shader::Create(ShaderName, VertexShader, FragmentShader);

        int samplers[m_Data.MaxTextureSlots];
        for (int i = 0; i < m_Data.MaxTextureSlots; i++)
            samplers[i] = i;

        m_Data.TextureShader->Bind();
        m_Data.TextureShader->SetIntArray("u_Textures", samplers, m_Data.MaxTextureSlots);

        m_Data.RectVertexBuffer = GraphicsBuffer::CreateVertexBuffer(m_Data.MaxVertices * sizeof(RectVertex));
        m_Data.RectVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TexCoord"},
            {ShaderDataType::Float, "a_TextureIndex"},
            {ShaderDataType::Float, "a_TilingFactor"}
        });

        uint32_t* rectangleIndices = new uint32_t[m_Data.MaxIndices];
        uint32_t offset = 0;
        for (int i = 0; i < m_Data.MaxIndices; i += 6)
        {
            rectangleIndices[i + 0] = offset + 0;
            rectangleIndices[i + 1] = offset + 1;
            rectangleIndices[i + 2] = offset + 2;

            rectangleIndices[i + 3] = offset + 2;
            rectangleIndices[i + 4] = offset + 3;
            rectangleIndices[i + 5] = offset + 0;

            offset += 4;
        }
        m_Data.VertexArray = VertexArray::Create(m_Data.RectVertexBuffer, GraphicsBuffer::CreateIndexBuffer(rectangleIndices, m_Data.MaxIndices * sizeof(uint32_t)));
        delete[] rectangleIndices;
        m_Data.BlankTexture = Texture2D::Create(1, 1);
        uint32_t blankTexturePixel = 0xffffffff;
        m_Data.BlankTexture->SetData(&blankTexturePixel, sizeof(uint32_t));
        memset(m_Data.TextureSlots.data(), 0, m_Data.MaxTextureSlots * sizeof(SharedPointer<Texture2D>));
        m_Data.TextureSlots[0] = m_Data.BlankTexture;
        //m_Data.TextureSlots.push_back(m_Data.BlankTexture);

        m_Data.RectVertexPositions[0] = { -0.5f, -0.5f, 0.0f , 1.0f };
        m_Data.RectVertexPositions[1] = {  0.5f, -0.5f, 0.0f , 1.0f };
        m_Data.RectVertexPositions[2] = {  0.5f,  0.5f, 0.0f , 1.0f };
        m_Data.RectVertexPositions[3] = { -0.5f,  0.5f, 0.0f , 1.0f };
    }

    void OpenGLRenderer2DAPI::BeginScene()
    {
        m_Data.CurrentVertex = m_Data.RectVerticesBuffer;
        m_Data.RectIndexCount = 0;

        m_Data.TextureSlotIndex = 1;
        memset(m_Data.TextureSlots.data() + 1, 0, (m_Data.MaxTextureSlots - 1)  * sizeof(SharedPointer<Texture2D>));
        /*for(int i = 1; i < m_Data.MaxTextureSlots; i++)
            m_Data.TextureSlots[i] = nullptr;*/
    }

    void OpenGLRenderer2DAPI::EndScene()
    {
        uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.CurrentVertex - (uint8_t*)m_Data.RectVerticesBuffer);
        m_Data.RectVertexBuffer->SetData(m_Data.RectVerticesBuffer, dataSize);

        Flush();
    }

    void OpenGLRenderer2DAPI::Flush()
    {
        if (m_Data.RectIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.CurrentVertex - (uint8_t*)m_Data.RectVerticesBuffer);
            m_Data.RectVertexBuffer->SetData(m_Data.RectVerticesBuffer, dataSize);

            // Bind textures
            for (uint32_t i = 0; i < m_Data.TextureSlotIndex; i++)
                m_Data.TextureSlots[i]->Bind(i);

            m_Data.TextureShader->Bind();
            Renderer::DrawIndexed(m_Data.VertexArray, m_Data.RectIndexCount);
            m_Data.Stats->DrawCalls++;
        }
    }

    void OpenGLRenderer2DAPI::DrawRectangle(float x, float y, float z, float width, float height, const Color4 &color,
                                            float rotation)
    {
        static const float blankTextureIndex = 0.0f;
        static const float TilingFactor = 1.0f;

        FlushAndReset();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(width, height, 1.0f));

        for (int i = 0; i < RectVertexCount; i++)
        {
            m_Data.CurrentVertex->Position = transform * m_Data.RectVertexPositions[i];
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
        if(m_Data.RectIndexCount >= m_Data.MaxIndices)
        {
            EndScene();
            BeginScene();
        }
    }

    void
    OpenGLRenderer2DAPI::DrawImage(float x, float y, float z, float width, float height, const Ref<Texture2D> &texture,
                                   float rotation, const Color4 &color, float textureMultiplier)
    {
        FlushAndReset();

        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < m_Data.TextureSlotIndex; i++)
        {
            if (*m_Data.TextureSlots[i].Get() == *texture.Get())
            {
                textureIndex = (float)i;
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            if (m_Data.TextureSlotIndex >= m_Data.MaxTextureSlots)
                FlushAndReset();
            textureIndex = (float)m_Data.TextureSlotIndex;
            m_Data.TextureSlots[m_Data.TextureSlotIndex] = texture;
            m_Data.TextureSlotIndex++;
        }

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(width, height, 1.0f));

        for (int i = 0; i < RectVertexCount; i++)
        {
            m_Data.CurrentVertex->Position = transform * m_Data.RectVertexPositions[i];
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
        m_Data.TextureShader->Bind();
        m_Data.TextureShader->SetMat4("u_ViewProjection", transform);
    }
}