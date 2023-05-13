//
// Created by alexl on 13.05.2023.
//

#include "BufferLayout.h"


namespace BeeEngine
{
    BufferElement::BufferElement(ShaderDataType type, const String &name, bool normalized)
    {

    }

    uint32_t BufferElement::GetSizeOfType(ShaderDataType type)
    {

    }

    BufferLayout::BufferLayout(const std::initializer_list<BufferElement> &elements)
    : m_Elements(elements)
    {
        CalculateOffsetsAndStride();
    }

    void BufferLayout::CalculateOffsetsAndStride()
    {
        int offset = 0;
        m_Stride = 0;
        for (auto &element : m_Elements)
        {
            element.SetOffset(offset);
            offset += element.GetSize();
            m_Stride += element.GetSize();
        }
    }
}
