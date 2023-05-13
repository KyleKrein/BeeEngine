//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/Shader.h"

namespace BeeEngine
{
    class OpenGLShader: public Shader
    {
    public:
        OpenGLShader(const String &name, const String &vertexSrc, const String &fragmentSrc);
        virtual ~OpenGLShader() override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetInt(const String &name, int value) override;
        virtual void SetIntArray(const String &name, int *values, uint32_t count) override;
        virtual void SetFloat(const String &name, float value) override;
        virtual void SetFloat2(const String &name, const glm::vec2 &value) override;
        virtual void SetFloat3(const String &name, const glm::vec3 &value) override;
        virtual void SetFloat4(const String &name, const glm::vec4 &value) override;
        virtual void SetMat4(const String &name, const glm::mat4 &value) override;

        virtual const String &GetName() const override { return m_Name; }

    private:
        uint32_t m_RendererID;
        String m_Name;
    };
}
