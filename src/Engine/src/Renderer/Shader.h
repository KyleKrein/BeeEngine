//
// Created by alexl on 13.05.2023.
//

#pragma once

#include <unordered_map>
#include "Core/TypeDefines.h"
#include "glm/glm.hpp"
#include "gsl/gsl"
#include "ShaderTypes.h"

namespace BeeEngine
{

    class Shader
    {
public:
        Shader() = default;
        virtual ~Shader() {};

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(std::string_view name, int value) = 0;
        virtual void SetIntArray(std::string_view name, gsl::span<int> values) = 0;
        virtual void SetFloat(std::string_view name, float value) = 0;
        virtual void SetFloat2(std::string_view name, const glm::vec2 &value) = 0;
        virtual void SetFloat3(std::string_view name, const glm::vec3& value) = 0;
        virtual void SetFloat4(std::string_view name, const glm::vec4& value) = 0;
        virtual void SetMat4(std::string_view name, const glm::mat4& value) = 0;

        virtual const std::string_view GetName() const = 0;

        static Ref<Shader> Create(std::string_view name, std::string_view vertexSrc, std::string_view fragmentSrc);
        static Ref<Shader> Create(std::string_view name, std::string_view filepath);
    private:


    };
}
