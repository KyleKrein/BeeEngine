//
// Created by alexl on 13.05.2023.
//

#include <vector>
#include "OpenGLShader.h"
#include "glad/glad.h"
#include "Core/Logging/Log.h"
#include <fstream>
#include "streambuf"
#include "gtc/type_ptr.hpp"
#include "Debug/OpenGLDebug.h"
#include "Debug/Instrumentor.h"


namespace BeeEngine::Internal
{

    OpenGLShader::OpenGLShader(std::string_view name, std::string_view vertexSrc, std::string_view fragmentSrc)
    : m_Name(name)
    {
        BEE_PROFILE_FUNCTION();
        std::unordered_map<ShaderType, Ref<String>> shaders;
        shaders[ShaderType::Vertex] = CreateRef<String>(vertexSrc);
        shaders[ShaderType::Fragment] = CreateRef<String>(fragmentSrc);
        Compile(shaders);
    }

    OpenGLShader::OpenGLShader(std::string_view name, std::string_view filepath)
    : m_Name(name)
    {
        BEE_PROFILE_FUNCTION();
        std::ifstream text((String(filepath)));
        String shaderSource;

        text.seekg(0, std::ios::end);
        shaderSource.reserve(text.tellg());
        text.seekg(0, std::ios::beg);

        shaderSource.assign((std::istreambuf_iterator<char>(text)),
                            std::istreambuf_iterator<char>());

        auto shaders = Preprocess(shaderSource);
        Compile(shaders);
    }

    OpenGLShader::~OpenGLShader()
    {
        BEE_PROFILE_FUNCTION();
        glDeleteProgram(m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::Bind() const
    {
        BEE_PROFILE_FUNCTION();
        glUseProgram(m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::Unbind() const
    {
        BEE_PROFILE_FUNCTION();
        glUseProgram(0);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::SetInt(std::string_view name, int value)
    {
        BEE_PROFILE_FUNCTION();
        int location = glGetUniformLocation(m_RendererID, name.data());
        BeeCoreAssert(location != -1, "Could not find uniform {0} in shader {1}", name, m_Name);
        glUniform1i(location, value);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::SetIntArray(std::string_view name, int *values, uint32_t count)
    {
        BEE_PROFILE_FUNCTION();
        int location = glGetUniformLocation(m_RendererID, name.data());
        BeeCoreAssert(location != -1, "Could not find uniform {0} in shader {1}", name, m_Name);
        glUniform1iv(location, count, values);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::SetFloat(std::string_view name, float value)
    {
        BEE_PROFILE_FUNCTION();
        int location = glGetUniformLocation(m_RendererID, name.data());
        BeeCoreAssert(location != -1, "Could not find uniform {0} in shader {1}", name, m_Name);
        glUniform1f(location, value);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::SetFloat2(std::string_view name, const glm::vec2 &value)
    {
        BEE_PROFILE_FUNCTION();
        int location = glGetUniformLocation(m_RendererID, name.data());
        BeeCoreAssert(location != -1, "Could not find uniform {0} in shader {1}", name, m_Name);
        glUniform2f(location, value.x, value.y);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::SetFloat3(std::string_view name, const glm::vec3 &value)
    {
        BEE_PROFILE_FUNCTION();
        int location = glGetUniformLocation(m_RendererID, name.data());
        BeeCoreAssert(location != -1, "Could not find uniform {0} in shader {1}", name, m_Name);
        glUniform3f(location, value.x, value.y, value.z);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::SetFloat4(std::string_view name, const glm::vec4 &value)
    {
        BEE_PROFILE_FUNCTION();
        int location = glGetUniformLocation(m_RendererID, name.data());
        BeeCoreAssert(location != -1, "Could not find uniform {0} in shader {1}", name, m_Name);
        glUniform4f(location, value.x, value.y, value.z, value.w);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::SetMat4(std::string_view name, const glm::mat4 &value)
    {
        BEE_PROFILE_FUNCTION();
        int location = glGetUniformLocation(m_RendererID, name.data());
        BeeCoreAssert(location != -1, "Could not find uniform {0} in shader {1}", name, m_Name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        OPENGL_CHECK_ERRORS
    }

    void OpenGLShader::Compile(const std::unordered_map<ShaderType, Ref<String>> &shaders)
    {
        BEE_PROFILE_FUNCTION();
        int program = glCreateProgram();
        std::vector<uint32_t> shaderIDs;
        shaderIDs.reserve(shaders.size());
        int success = 0;
        for (auto& [type, source] : shaders)
        {
            if(source == nullptr || source->empty() || source->length() == 0)
                continue;
            uint32_t shader = glCreateShader(type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
            const char* src = source->c_str();
            glShaderSource(shader, 1, &src, nullptr);
            glCompileShader(shader);

            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (success == GL_FALSE)
            {
                int maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<char> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(shader);

                BeeCoreError("{0}", infoLog.data());
                BeeCoreAssert(false, "Shader compilation failure!");
                return;
            }

            glAttachShader(program, shader);
            shaderIDs.push_back(shader);
        }

        BeeCoreAssert(shaderIDs.size() > 0, "No shaders were compiled!");

        m_RendererID = program;

        for(auto& shader : shaderIDs)
            glAttachShader(program, shader);

        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (success == GL_FALSE)
        {
            int maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<char> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(program);

            for(auto& shader : shaderIDs)
                glDeleteShader(shader);

            BeeCoreError("{0}", infoLog.data());
            BeeCoreAssert(false, "Shader link failure!");
            return;
        }

        for(auto& shader : shaderIDs)
        {
            glDetachShader(program, shader);
            glDeleteShader(shader);
        }
        OPENGL_CHECK_ERRORS
    }

    std::unordered_map<ShaderType, Ref<String>> OpenGLShader::Preprocess(const String &shaderSource)
    {
        BEE_PROFILE_FUNCTION();
        std::unordered_map<ShaderType, Ref<String>> shaders;
        shaders.reserve(2);
        size_t pos = shaderSource.find("#type");
        while(pos != String::npos)
        {
            size_t eol = shaderSource.find_first_of("\r\n", pos);
            BeeCoreAssert(eol != String::npos, "Syntax error");
            size_t begin = pos + 6;
            String type = shaderSource.substr(begin, eol - begin);
            BeeCoreAssert(type == "vertex" || type == "fragment", "Invalid shader type specified");
            size_t nextLinePos = shaderSource.find_first_not_of("\r\n", eol);
            pos = shaderSource.find("#type", nextLinePos);
            shaders[type == "vertex" ? ShaderType::Vertex : ShaderType::Fragment] = CreateRef<String>(shaderSource.substr(nextLinePos,
                                                                                                                           pos - (nextLinePos == String::npos ? shaderSource.size() - 1 : nextLinePos)));
        }
        return shaders;
    }

}
