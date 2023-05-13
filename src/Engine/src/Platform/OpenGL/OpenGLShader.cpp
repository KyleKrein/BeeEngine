//
// Created by alexl on 13.05.2023.
//

#include "OpenGLShader.h"


namespace BeeEngine
{

    OpenGLShader::OpenGLShader(const String &name, const String &vertexSrc, const String &fragmentSrc)
    {
        m_Name = name;

    }

    OpenGLShader::~OpenGLShader()
    {

    }

    void OpenGLShader::Bind() const
    {

    }

    void OpenGLShader::Unbind() const
    {

    }

    void OpenGLShader::SetInt(const String &name, int value)
    {

    }

    void OpenGLShader::SetIntArray(const String &name, int *values, uint32_t count)
    {

    }

    void OpenGLShader::SetFloat(const String &name, float value)
    {

    }

    void OpenGLShader::SetFloat2(const String &name, const glm::vec2 &value)
    {

    }

    void OpenGLShader::SetFloat3(const String &name, const glm::vec3 &value)
    {

    }

    void OpenGLShader::SetFloat4(const String &name, const glm::vec4 &value)
    {

    }

    void OpenGLShader::SetMat4(const String &name, const glm::mat4 &value)
    {

    }
}
