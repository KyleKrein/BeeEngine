//
// Created by alexl on 13.05.2023.
//

#pragma once

#include <initializer_list>
#include <vector>
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    enum class ShaderDataType
    {
        NoneData = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    struct BufferElement
    {
        BufferElement() = default;

        BufferElement(ShaderDataType type, const String &name, bool normalized = false);

        [[nodiscard]] inline uint32_t GetSize() const
        { return m_Size; }

        [[nodiscard]] inline uint32_t GetOffset() const
        { return m_Offset; }
        inline void SetOffset(uint32_t offset)
        { m_Offset = offset; }

        [[nodiscard]] inline const String &GetName() const
        { return m_Name; }

        [[nodiscard]] inline ShaderDataType GetType() const
        { return m_Type; }

        [[nodiscard]] inline bool IsNormalized() const
        { return m_Normalized; }

        static uint32_t GetSizeOfType(ShaderDataType type);
        [[nodiscard]] uint32_t GetComponentCount() const;

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

        [[nodiscard]] inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }
        [[nodiscard]] inline uint32_t GetStride() const { return m_Stride; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

        BufferLayout() = default;

    private:
        uint32_t m_Stride;
        std::vector<BufferElement> m_Elements;

        void CalculateOffsetsAndStride();
    };
}
