//
// Created by Александр Лебедев on 16.10.2023.
//

#include "Reflection.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace BeeEngine::Reflection
{
    struct TypeDescriptor_int32_t : TypeDescriptor
    {
        TypeDescriptor_int32_t() : TypeDescriptor{"int32_t", sizeof(int32_t)} {}
    };

    template <>
    TypeDescriptor& GetPrimitiveDescriptor<int32_t>()
    {
        static TypeDescriptor_int32_t typeDesc;
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<int64_t>()
    {
        static TypeDescriptor typeDesc{"int64_t", sizeof(int64_t)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<uint64_t>()
    {
        static TypeDescriptor typeDesc{"uint64_t", sizeof(uint64_t)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<uint32_t>()
    {
        static TypeDescriptor typeDesc{"uint32_t", sizeof(uint32_t)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<float>()
    {
        static TypeDescriptor typeDesc{"float", sizeof(float)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<double>()
    {
        static TypeDescriptor typeDesc{"double", sizeof(double)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<bool>()
    {
        static TypeDescriptor typeDesc{"bool", sizeof(bool)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<String>()
    {
        static TypeDescriptor typeDesc{"String", sizeof(String)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<glm::vec4>()
    {
        static TypeDescriptor typeDesc{"glm::vec4", sizeof(glm::vec4)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<glm::vec3>()
    {
        static TypeDescriptor typeDesc{"glm::vec3", sizeof(glm::vec3)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<glm::vec2>()
    {
        static TypeDescriptor typeDesc{"glm::vec2", sizeof(glm::vec2)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<glm::mat4>()
    {
        static TypeDescriptor typeDesc{"glm::mat4", sizeof(glm::mat4)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<glm::mat3>()
    {
        static TypeDescriptor typeDesc{"glm::mat3", sizeof(glm::mat3)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<glm::mat2>()
    {
        static TypeDescriptor typeDesc{"glm::mat2", sizeof(glm::mat2)};
        return typeDesc;
    }
    template <>
    TypeDescriptor& GetPrimitiveDescriptor<glm::quat>()
    {
        static TypeDescriptor typeDesc{"glm::quat", sizeof(glm::quat)};
        return typeDesc;
    }
} // namespace BeeEngine::Reflection