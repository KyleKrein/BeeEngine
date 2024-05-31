//
// Created by alexl on 13.05.2023.
//

#include "BufferLayout.h"
#include "Debug/Instrumentor.h"

namespace BeeEngine
{
    BufferElement::BufferElement(ShaderDataType type, const String& name, uint32_t location, bool normalized)
        : m_Name(name),
          m_Type(type),
          m_Size(GetSizeOfType(type)),
          m_Offset(0),
          m_Location(location),
          m_Normalized(normalized)
    {
    }

    uint32_t BufferElement::GetSizeOfType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
                return 4;
            case ShaderDataType::Float2:
                return 4 * 2;
            case ShaderDataType::Float3:
                return 4 * 3;
            case ShaderDataType::Float4:
                return 4 * 4;
            case ShaderDataType::Mat3:
                return 4 * 3 * 3;
            case ShaderDataType::Mat4:
                return 4 * 4 * 4;
            case ShaderDataType::Int:
                return 4;
            case ShaderDataType::Int2:
                return 4 * 2;
            case ShaderDataType::Int3:
                return 4 * 3;
            case ShaderDataType::Int4:
                return 4 * 4;
            case ShaderDataType::Bool:
                return 1;
            case ShaderDataType::NoneData:
                break;
        }
        return 0;
    }

    uint32_t BufferElement::GetComponentCount() const
    {
        switch (m_Type)
        {
            case ShaderDataType::Float:
                return 1;
            case ShaderDataType::Float2:
                return 2;
            case ShaderDataType::Float3:
                return 3;
            case ShaderDataType::Float4:
                return 4;
            case ShaderDataType::Mat3:
                return 3 * 3;
            case ShaderDataType::Mat4:
                return 4 * 4;
            case ShaderDataType::Int:
                return 1;
            case ShaderDataType::Int2:
                return 2;
            case ShaderDataType::Int3:
                return 3;
            case ShaderDataType::Int4:
                return 4;
            case ShaderDataType::Bool:
                return 1;
            case ShaderDataType::NoneData:
                break;
        }
        return 0;
    }

    BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Stride(0), m_Elements(elements)
    {
        CalculateOffsetsAndStride();
    }

    void BufferLayout::CalculateOffsetsAndStride()
    {
        BEE_PROFILE_FUNCTION();
        uint32_t offset = 0;
        for (auto& element : m_Elements)
        {
            element.SetOffset(offset);
            offset += element.GetSize();
            m_Stride += element.GetSize();
        }
    }

    BufferLayout::BufferLayout(std::vector<BufferElement>&& inElements,
                               std::vector<BufferElement>&& instanceElements,
                               std::vector<BufferUniformElement>&& uniformElements,
                               std::vector<BufferElement>&& outElements)
        : m_Stride(0),
          m_InstancedStride(0),
          m_InElements(std::move(inElements)),
          m_InstancedElements(std::move(instanceElements)),
          m_UniformElements(std::move(uniformElements)),
          m_OutElements(std::move(outElements))
    {
        BEE_PROFILE_FUNCTION();
        uint32_t offset = 0;
        for (auto& element : m_InElements)
        {
            element.SetOffset(offset);
            offset += element.GetSize();
            m_Stride += element.GetSize();
        }

        offset = 0;
        for (auto& element : m_InstancedElements)
        {
            element.SetOffset(offset);
            offset += element.GetSize();
            m_InstancedStride += element.GetSize();
        }
    }
} // namespace BeeEngine
