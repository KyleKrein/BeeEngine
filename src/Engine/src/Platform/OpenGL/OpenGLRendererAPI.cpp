//
// Created by Александр Лебедев on 07.05.2023.
//

#include "OpenGLRendererAPI.h"
#include "glad/glad.h"
#include "Renderer/Renderer2D.h"
#include "OpenGLRenderer2DAPI.h"
#include "Core/Logging/Log.h"
#include "Debug/OpenGLDebug.h"

namespace BeeEngine
{
    void OpenGLMessageCallback(
            unsigned source,
            unsigned type,
            unsigned id,
            unsigned severity,
            int length,
            const char* message,
            const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         BeeCoreError(message); return;
            case GL_DEBUG_SEVERITY_MEDIUM:       BeeCoreError(message); return;
            case GL_DEBUG_SEVERITY_LOW:          BeeCoreWarn(message); return;
            case GL_DEBUG_SEVERITY_NOTIFICATION: BeeCoreTrace(message); return;
        }

        BeeCoreAssert(false, "Unknown severity level!");
    }

    void OpenGLRendererAPI::Init()
    {
#ifdef DEBUG
#ifndef MACOS
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
#endif

        String vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        String renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        String version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        String shadingLanguageVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        BeeCoreTrace("OpenGL Initialized: \n\tVendor: {0}\n\tRenderer: {1}\n\tVersion: {2}\n\tShading language version: {3}", vendor, renderer, version, shadingLanguageVersion);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        OPENGL_CHECK_ERRORS

        Renderer2D::Init(new OpenGLRenderer2DAPI());
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLRendererAPI::SetClearColor(const Color4& color)
    {
        float r = color.R();
        float g = color.G();
        float b = color.B();
        float a = color.A();
        glClearColor(*((GLfloat*)(&r)), *((GLfloat*)(&g)), *((GLfloat*)(&b)), *((GLfloat*)(&a)));
        OPENGL_CHECK_ERRORS
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t indexCount)
    {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        OPENGL_CHECK_ERRORS
    }
}