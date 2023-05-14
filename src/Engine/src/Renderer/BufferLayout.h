//
// Created by alexl on 13.05.2023.
//

#pragma once

#include <initializer_list>
#include <vector>
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    enum ShaderDataType
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    struct BufferElement
    {
        BufferElement() = default;

        BufferElement(ShaderDataType type, const String &name, bool normalized = false);

        inline uint32_t GetSize() const
        { return m_Size; }

        inline uint32_t GetOffset() const
        { return m_Offset; }
        inline void SetOffset(uint32_t offset)
        { m_Offset = offset; }

        inline const String &GetName() const
        { return m_Name; }

        inline ShaderDataType GetType() const
        { return m_Type; }

        inline bool IsNormalized() const
        { return m_Normalized; }

        static uint32_t GetSizeOfType(ShaderDataType type);
        uint32_t GetComponentCount();

    private:
        String m_Name;
        ShaderDataType m_Type;
        uint32_t m_Size;
        uint32_t m_Offset;
        bool m_Normalized;


    };

    struct BufferLayout
    {
        BufferLayout(const std::initializer_list<BufferElement> &elements);

        inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }
        inline uint32_t GetStride() const { return m_Stride; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    private:
        int m_Stride;
        std::vector<BufferElement> m_Elements;
        BufferLayout() = default;

        void CalculateOffsetsAndStride();
    };
}
