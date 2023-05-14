//
// Created by alexl on 13.05.2023.
//

#pragma once

#include <unordered_map>
#include "Core/TypeDefines.h"
#include "glm/glm.hpp"

namespace BeeEngine
{
    enum ShaderType
    {
        Vertex = 0,
        Fragment = 1
    };
    class Shader
    {
public:
        Shader() = default;
        virtual ~Shader() {};

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const String& name, int value) = 0;
        virtual void SetIntArray(const String& name, int* values, uint32_t count) = 0;
        virtual void SetFloat(const String& name, float value) = 0;
        virtual void SetFloat2(const String &name, const glm::vec2 &value) = 0;
        virtual void SetFloat3(const String& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const String& name, const glm::vec4& value) = 0;
        virtual void SetMat4(const String& name, const glm::mat4& value) = 0;

        virtual const Ref<String> GetName() const = 0;

        static Ref<Shader> Create(Ref<String> name, const String& vertexSrc, const String& fragmentSrc);
        static Ref<Shader> Create(Ref<String> name, const String& filepath);
    private:


    };
}
