//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/Shader.h"

namespace BeeEngine::Internal
{
    class OpenGLShader: public Shader
    {
    public:
        OpenGLShader(std::string_view name, std::string_view vertexSrc, std::string_view fragmentSrc);
        OpenGLShader(std::string_view name, std::string_view filepath);
        virtual ~OpenGLShader() override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetInt(std::string_view name, int value) override;
        virtual void SetIntArray(std::string_view name, int *values, uint32_t count) override;
        virtual void SetFloat(std::string_view name, float value) override;
        virtual void SetFloat2(std::string_view name, const glm::vec2 &value) override;
        virtual void SetFloat3(std::string_view name, const glm::vec3 &value) override;
        virtual void SetFloat4(std::string_view name, const glm::vec4 &value) override;
        virtual void SetMat4(std::string_view name, const glm::mat4 &value) override;

        virtual const std::string_view GetName() const override { return m_Name; }

    private:
        void Compile(const std::unordered_map<ShaderType, Ref<String>> &shaders);
        std::unordered_map<ShaderType, Ref<String>> Preprocess(const String &shaderSource);
        uint32_t m_RendererID;
        String m_Name;
    };
}
