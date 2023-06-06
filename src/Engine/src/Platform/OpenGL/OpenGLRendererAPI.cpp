//
// Created by Александр Лебедев on 07.05.2023.
//

#include "OpenGLRendererAPI.h"
#include "glad/glad.h"
#include "Renderer/Renderer2D.h"
#include "OpenGLRenderer2DAPI.h"
#include "Core/Logging/Log.h"
#include "Debug/OpenGLDebug.h"
#include "Debug/Instrumentor.h"
#include "Utils/Expects.h"

namespace BeeEngine::Internal
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
        BEE_PROFILE_FUNCTION();
#ifdef DEBUG
#ifndef MACOS
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
#endif

        std::string_view vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        std::string_view renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        std::string_view version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::string_view shadingLanguageVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

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
        BEE_PROFILE_FUNCTION();
        BeeExpects(width > 0 && height > 0 && width < 100000 && height < 100000);
        glViewport(gsl::narrow_cast<int>(x),
                gsl::narrow_cast<int>(y),
                gsl::narrow_cast<int>(width),
                gsl::narrow_cast<int>(height));
        OPENGL_CHECK_ERRORS
    }

    void OpenGLRendererAPI::SetClearColor(const Color4& color)
    {
        BEE_PROFILE_FUNCTION();
        GLfloat r = color.R();
        GLfloat g = color.G();
        GLfloat b = color.B();
        GLfloat a = color.A();
        glClearColor(r, g, b, a);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLRendererAPI::Clear()
    {
        BEE_PROFILE_FUNCTION();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t indexCount)
    {
        BEE_PROFILE_FUNCTION();
        glDrawElements(GL_TRIANGLES, gsl::narrow_cast<GLint>(indexCount), GL_UNSIGNED_INT, nullptr);
        OPENGL_CHECK_ERRORS
    }

    Color4 OpenGLRendererAPI::ReadPixel(uint32_t x, uint32_t y)
    {
        GLubyte pixel[4];
        glReadPixels(gsl::narrow_cast<GLint>(x), gsl::narrow_cast<GLint>(y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
        return Color4::FromRGBA(pixel[0], pixel[1], pixel[2], pixel[3]);
    }
}