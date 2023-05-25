//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/GraphicsBuffer.h"
#include "Renderer/VertexArray.h"
#include "glad/glad.h"
#include "Core/Logging/Log.h"

namespace BeeEngine::Internal
{
    class OpenGLVertexArray: public VertexArray
    {
    public:
        OpenGLVertexArray(Ref<GraphicsBuffer> vertexBuffer, Ref<GraphicsBuffer> indexBuffer);
        ~OpenGLVertexArray() override;

        void Bind() const override;
        void Unbind() const override;
    private:
        uint32_t m_RendererID;
        Ref<GraphicsBuffer> m_VertexBuffer;
        Ref<GraphicsBuffer> m_IndexBuffer;
        static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
        {
            switch (type)
            {
                case ShaderDataType::Float:    return GL_FLOAT;
                case ShaderDataType::Float2:   return GL_FLOAT;
                case ShaderDataType::Float3:   return GL_FLOAT;
                case ShaderDataType::Float4:   return GL_FLOAT;
                case ShaderDataType::Mat3:     return GL_FLOAT;
                case ShaderDataType::Mat4:     return GL_FLOAT;
                case ShaderDataType::Int:      return GL_INT;
                case ShaderDataType::Int2:     return GL_INT;
                case ShaderDataType::Int3:     return GL_INT;
                case ShaderDataType::Int4:     return GL_INT;
                case ShaderDataType::Bool:     return GL_BOOL;
            }
            BeeCoreError("Unknown ShaderDataType!");
            return 0;
        }
    };
}
