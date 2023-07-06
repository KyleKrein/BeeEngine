//
// Created by alexl on 13.05.2023.
//

#pragma once

#include <initializer_list>
#include <vector>
#include "Core/TypeDefines.h"
#include <algorithm>

namespace BeeEngine
{
    enum class ShaderDataType
    {
        NoneData = 0,
        Half,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        UInt, UInt2, UInt3, UInt4,
        Int, Int2, Int3, Int4,
        Bool
    };
    class BufferLayoutBuilder;
    struct BufferElement
    {
        friend class BufferLayoutBuilder;
        BufferElement() = default;

        BufferElement(ShaderDataType type, const String &name, uint32_t location = 0, bool normalized = false);

        [[nodiscard]] inline uint32_t GetSize() const
        { return m_Size; }

        [[nodiscard]] inline uint32_t GetOffset() const
        { return m_Offset; }
        inline void SetOffset(uint32_t offset)
        { m_Offset = offset; }
        inline uint32_t GetLocation() const
        {
            return m_Location;
        }

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
        uint32_t m_Location;
        bool m_Normalized;
    };

    class BufferLayoutBuilder;

    struct BufferLayout
    {
        BufferLayout(const std::initializer_list<BufferElement> &elements);
        BufferLayout(std::vector<BufferElement>&& inElements, std::vector<BufferElement>&& outElements);

        [[nodiscard]] inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }
        [[nodiscard]] inline const std::vector<BufferElement> &GetInputElements() const { return m_InElements; }
        [[nodiscard]] inline uint32_t GetStride() const { return m_Stride; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

        BufferLayout() = default;

    private:
        uint32_t m_Stride;
        std::vector<BufferElement> m_Elements;
        std::vector<BufferElement> m_InElements;
        std::vector<BufferElement> m_OutElements;

        void CalculateOffsetsAndStride();
    };

    class BufferLayoutBuilder
    {
    public:
        void NumberOfInputs(uint32_t count)
        {
            m_InElements.reserve(count);
        }
        void NumberOfOutputs(uint32_t count)
        {
            m_OutElements.reserve(count);
        }
        void AddInput(ShaderDataType type, const String& name, uint32_t location, bool normalized = false)
        {
#if 0
            if(!m_Temp.empty() && !(name.contains(m_Temp[0].GetName()) && type == m_Temp[0].GetType()))
            {
                Flush();
            }
            m_Temp.emplace_back(type, name, normalized);
            return;
#endif
            BeeCoreTrace("Registered in element of type {0} with name {1} in location {2}", ToString(type), name, location);
            m_InElements.emplace_back(type, name, location, normalized);
        }
        void AddOutput(ShaderDataType type, const String& name, uint32_t location, bool normalized = false)
        {
            m_OutElements.emplace_back(type, name, location, normalized);
        }
        BufferLayout Build()
        {
            //Flush();
            std::sort(m_InElements.begin(), m_InElements.end(), [](const BufferElement& a, const BufferElement& b) -> bool
            {
                return a.GetLocation() < b.GetLocation();
            });
            BeeCoreTrace("Finished building layout");
            return BufferLayout(std::move(m_InElements), std::move(m_OutElements));
        }
    private:
        void Flush()
        {
            if(!m_Temp.empty())
            {
                BufferElement element = m_Temp[0];
                auto size = m_Temp.size();
                if(size > 1)
                {
                    switch (size)
                    {
                        case 4:
                            if(element.m_Type == ShaderDataType::Float4)
                                element.m_Type = ShaderDataType::Mat4;
                            break;
                        case 3:
                            if(element.m_Type == ShaderDataType::Float3)
                                element.m_Type = ShaderDataType::Mat3;
                        default:
                            BeeCoreError("Unsupported type");
                    }
                }
                BeeCoreTrace("Adding element {0} of type {1}", element.GetName(), ToString(element.GetType()));
                m_InElements.emplace_back(element);
                m_Temp.clear();
            }
        }
        std::vector<BufferElement> m_Temp;
        std::vector<BufferElement> m_InElements;
        std::vector<BufferElement> m_OutElements;
    };
}
