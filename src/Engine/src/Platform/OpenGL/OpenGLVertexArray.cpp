//
// Created by alexl on 13.05.2023.
//

#include "OpenGLVertexArray.h"
#include "glad/glad.h"
#include "Debug/OpenGLDebug.h"
#include "Debug/Instrumentor.h"


namespace BeeEngine::Internal
{

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"
    OpenGLVertexArray::OpenGLVertexArray(Ref<GraphicsBuffer> vertexBuffer, Ref<GraphicsBuffer> indexBuffer)
    : m_RendererID(0), m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer)
    {
        BEE_PROFILE_FUNCTION();
        glGenVertexArrays(1, &m_RendererID);
        OPENGL_CHECK_ERRORS
        glBindVertexArray(m_RendererID);
        OPENGL_CHECK_ERRORS

        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        uint32_t vertexBufferIndex = 0;
        for (const auto& element : layout)
        {
            switch (element.GetType())
            {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                {
                    glEnableVertexAttribArray(vertexBufferIndex);
                    glVertexAttribPointer(vertexBufferIndex,
                                          element.GetComponentCount(),
                                          ShaderDataTypeToOpenGLBaseType(element.GetType()),
                                          element.IsNormalized() ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.GetOffset());
                    vertexBufferIndex++;
                    break;
                }
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                {
                    glEnableVertexAttribArray(vertexBufferIndex);
                    glVertexAttribIPointer(vertexBufferIndex,
                                           element.GetComponentCount(),
                                           ShaderDataTypeToOpenGLBaseType(element.GetType()),
                                           layout.GetStride(),
                                           (const void*)element.GetOffset());
                    vertexBufferIndex++;
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                {
                    uint8_t count = element.GetComponentCount();
                    for (uint8_t i = 0; i < count; i++)
                    {
                        glEnableVertexAttribArray(vertexBufferIndex);
                        glVertexAttribPointer(vertexBufferIndex,
                                              count,
                                              ShaderDataTypeToOpenGLBaseType(element.GetType()),
                                              element.IsNormalized() ? GL_TRUE : GL_FALSE,
                                              layout.GetStride(),
                                              (const void*)(element.GetOffset() + sizeof(float) * count * i));
                        glVertexAttribDivisor(vertexBufferIndex, 1);
                        vertexBufferIndex++;
                    }
                    break;
                }
                default:
                    BeeCoreError("Unknown ShaderDataType!");
            }
        }
        OPENGL_CHECK_ERRORS
        indexBuffer->Bind();
    }
#pragma clang diagnostic pop

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        BEE_PROFILE_FUNCTION();
        glDeleteVertexArrays(1, &m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLVertexArray::Bind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindVertexArray(m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLVertexArray::Unbind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindVertexArray(0);
        OPENGL_CHECK_ERRORS
    }
}
