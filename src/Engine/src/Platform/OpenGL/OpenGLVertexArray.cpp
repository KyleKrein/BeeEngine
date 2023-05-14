//
// Created by alexl on 13.05.2023.
//

#include "OpenGLVertexArray.h"
#include "glad/glad.h"


namespace BeeEngine
{

    OpenGLVertexArray::OpenGLVertexArray(Ref<GraphicsBuffer> vertexBuffer, Ref<GraphicsBuffer> indexBuffer)
    : m_RendererID(0)
    {
        glGenVertexArrays(1, &m_RendererID);
        glBindVertexArray(m_RendererID);

        vertexBuffer->Bind();
        uint32_t index = 0;
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
                                              (const void*)(element.GetType() + sizeof(float) * count * i));
                        glVertexAttribDivisor(vertexBufferIndex, 1);
                        vertexBufferIndex++;
                    }
                    break;
                }
                default:
                    BeeCoreError("Unknown ShaderDataType!");
            }
        }
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }
}
