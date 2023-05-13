//
// Created by Александр Лебедев on 07.05.2023.
//

#include "OpenGLRendererAPI.h"
#include "glad/glad.h"

namespace BeeEngine
{

    void OpenGLRendererAPI::Init()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLRendererAPI::SetClearColor(const Color4& color)
    {
        float r = color.R();
        float g = color.G();
        float b = color.B();
        float a = color.A();
        glClearColor(*((GLfloat*)(&r)), *((GLfloat*)(&g)), *((GLfloat*)(&b)), *((GLfloat*)(&a)));
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t indexCount)
    {
        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
}