//
// Created by alexl on 13.05.2023.
//

#include "BufferLayout.h"


namespace BeeEngine
{
    BufferElement::BufferElement(ShaderDataType type, const String &name, bool normalized)
    : m_Name(name), m_Type(type), m_Size(GetSizeOfType(type)), m_Offset(0), m_Normalized(normalized)
    {

    }

    uint32_t BufferElement::GetSizeOfType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:    return 4;
            case ShaderDataType::Float2:   return 4 * 2;
            case ShaderDataType::Float3:   return 4 * 3;
            case ShaderDataType::Float4:   return 4 * 4;
            case ShaderDataType::Mat3:     return 4 * 3 * 3;
            case ShaderDataType::Mat4:     return 4 * 4 * 4;
            case ShaderDataType::Int:      return 4;
            case ShaderDataType::Int2:     return 4 * 2;
            case ShaderDataType::Int3:     return 4 * 3;
            case ShaderDataType::Int4:     return 4 * 4;
            case ShaderDataType::Bool:     return 1;
        }
        return 0;
    }

    uint32_t BufferElement::GetComponentCount() const
    {
        switch (m_Type)
        {
            case ShaderDataType::Float:    return 1;
            case ShaderDataType::Float2:   return 2;
            case ShaderDataType::Float3:   return 3;
            case ShaderDataType::Float4:   return 4;
            case ShaderDataType::Mat3:     return 3 * 3;
            case ShaderDataType::Mat4:     return 4 * 4;
            case ShaderDataType::Int:      return 1;
            case ShaderDataType::Int2:     return 2;
            case ShaderDataType::Int3:     return 3;
            case ShaderDataType::Int4:     return 4;
            case ShaderDataType::Bool:     return 1;
        }
        return 0;
    }

    BufferLayout::BufferLayout(const std::initializer_list<BufferElement> &elements)
    : m_Elements(elements), m_Stride(0)
    {
        CalculateOffsetsAndStride();
    }

    void BufferLayout::CalculateOffsetsAndStride()
    {
        int offset = 0;
        for (auto &element : m_Elements)
        {
            element.SetOffset(offset);
            offset += element.GetSize();
            m_Stride += element.GetSize();
        }
    }
}