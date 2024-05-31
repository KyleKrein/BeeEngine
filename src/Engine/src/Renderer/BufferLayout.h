//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include <algorithm>
#include <initializer_list>
#include <utility>
#include <vector>

namespace BeeEngine
{
    enum class ShaderDataType
    {
        NoneData = 0,
        Half,
        Float,
        Float2,
        Float3,
        Float4,
        Mat3,
        Mat4,
        UInt,
        UInt2,
        UInt3,
        UInt4,
        Int,
        Int2,
        Int3,
        Int4,
        Bool
    };
    enum class ShaderUniformDataType
    {
        Unknown = 0,
        Data,
        Sampler,
        SampledTexture
    };
    class BufferLayoutBuilder;
    struct BufferElement
    {
        friend class BufferLayoutBuilder;
        BufferElement() = default;

        BufferElement(ShaderDataType type, const String& name, uint32_t location = 0, bool normalized = false);

        [[nodiscard]] inline uint32_t GetSize() const { return m_Size; }

        [[nodiscard]] inline uint32_t GetOffset() const { return m_Offset; }
        inline void SetOffset(uint32_t offset) { m_Offset = offset; }
        inline uint32_t GetLocation() const { return m_Location; }

        [[nodiscard]] inline const String& GetName() const { return m_Name; }

        [[nodiscard]] inline ShaderDataType GetType() const { return m_Type; }

        [[nodiscard]] inline bool IsNormalized() const { return m_Normalized; }

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

    class BufferUniformElement
    {
    public:
        friend class BufferLayoutBuilder;
        BufferUniformElement() = default;

        BufferUniformElement(ShaderUniformDataType type, uint32_t bindingSet, uint32_t location, uint32_t size)
            : m_Type(type), m_Size(size), m_BindingSet(bindingSet), m_Location(location)
        {
        }

        [[nodiscard]] inline uint32_t GetSize() const { return m_Size; }
        inline uint32_t GetLocation() const { return m_Location; }
        inline uint32_t GetBindingSet() const { return m_BindingSet; }

        [[nodiscard]] inline ShaderUniformDataType GetType() const { return m_Type; }

    private:
        ShaderUniformDataType m_Type;
        uint32_t m_Size;
        uint32_t m_BindingSet;
        uint32_t m_Location;
    };

    class BufferLayoutBuilder;

    struct BufferLayout
    {
        BufferLayout(const std::initializer_list<BufferElement>& elements);
        BufferLayout(std::vector<BufferElement>&& inElements,
                     std::vector<BufferElement>&& instanceElements,
                     std::vector<BufferUniformElement>&& uniformElements,
                     std::vector<BufferElement>&& outElements);

        [[nodiscard]] inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
        [[nodiscard]] inline const std::vector<BufferElement>& GetInputElements() const { return m_InElements; }
        [[nodiscard]] inline const std::vector<BufferElement>& GetInstancedElements() const
        {
            return m_InstancedElements;
        }
        [[nodiscard]] inline const std::vector<BufferUniformElement>& GetUniformElements() const
        {
            return m_UniformElements;
        }
        [[nodiscard]] inline const std::vector<BufferElement>& GetOutputElements() const { return m_OutElements; }
        [[nodiscard]] inline uint32_t GetStride() const { return m_Stride; }
        [[nodiscard]] inline uint32_t GetInstancedStride() const { return m_InstancedStride; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

        BufferLayout() = default;

    private:
        uint32_t m_Stride;
        uint32_t m_InstancedStride;
        std::vector<BufferElement> m_Elements;
        std::vector<BufferElement> m_InElements;
        std::vector<BufferElement> m_InstancedElements;
        std::vector<BufferUniformElement> m_UniformElements;
        std::vector<BufferElement> m_OutElements;

        void CalculateOffsetsAndStride();
    };

    class BufferLayoutBuilder
    {
    public:
        void NumberOfInputs(uint32_t count)
        {
            // m_InElements.reserve(count);
        }
        void RegisterInstancedLocation(uint32_t location) { m_InstancedLocations.emplace_back(location); }
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
            if (IsInstanced(location))
            {
                BeeCoreTrace("Registered in instanced element of type {0} with name {1} in location {2}",
                             ToString(type),
                             name,
                             location);
                m_InstancedElements.emplace_back(type, name, location, normalized);
                return;
            }
            BeeCoreTrace(
                "Registered in element of type {0} with name {1} in location {2}", ToString(type), name, location);
            m_InElements.emplace_back(type, name, location, normalized);
        }
        void AddUniform(ShaderUniformDataType type, uint32_t bindingSet, uint32_t location, uint32_t size)
        {
            for (auto& element : m_UniformElements)
            {
                if (element.GetBindingSet() == bindingSet && element.GetLocation() == location)
                {
                    BeeExpects(type == element.GetType());
                    if (element.GetSize() != size)
                    {
                        element = BufferUniformElement(type, bindingSet, location, size);
                    }
                    return;
                }
            }
            BeeCoreTrace("Registered uniform element of type {0} in binding set {1} and location {2} of size {3}",
                         ToString(type),
                         bindingSet,
                         location,
                         size);
            m_UniformElements.emplace_back(type, bindingSet, location, size);
        }

        void AddOutput(ShaderDataType type, const String& name, uint32_t location, bool normalized = false)
        {
            BeeCoreTrace(
                "Registered out element of type {0} with name {1} in location {2}", ToString(type), name, location);
            m_OutElements.emplace_back(type, name, location, normalized);
        }

        [[nodiscard]]
        BufferLayout Build()
        {
            // Flush();
            std::sort(m_InElements.begin(),
                      m_InElements.end(),
                      [](const BufferElement& a, const BufferElement& b) -> bool
                      { return a.GetLocation() < b.GetLocation(); });
            std::sort(m_InstancedElements.begin(),
                      m_InstancedElements.end(),
                      [](const BufferElement& a, const BufferElement& b) -> bool
                      { return a.GetLocation() < b.GetLocation(); });
            std::sort(m_UniformElements.begin(),
                      m_UniformElements.end(),
                      [](const BufferUniformElement& a, const BufferUniformElement& b) -> bool
                      { return a.GetBindingSet() < b.GetBindingSet() || a.GetLocation() < b.GetLocation(); });
            std::ranges::sort(m_OutElements,
                              [](const BufferElement& a, const BufferElement& b)
                              { return a.GetLocation() < b.GetLocation(); });
            BeeCoreTrace("Finished building layout");
            return BufferLayout(std::move(m_InElements),
                                std::move(m_InstancedElements),
                                std::move(m_UniformElements),
                                std::move(m_OutElements));
        }

        void NumberOfOutputs(size_t size) { m_OutElements.reserve(size); }

    private:
        void Flush()
        {
            if (!m_Temp.empty())
            {
                BufferElement element = m_Temp[0];
                auto size = m_Temp.size();
                if (size > 1)
                {
                    switch (size)
                    {
                        case 4:
                            if (element.m_Type == ShaderDataType::Float4)
                                element.m_Type = ShaderDataType::Mat4;
                            break;
                        case 3:
                            if (element.m_Type == ShaderDataType::Float3)
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
        bool IsInstanced(uint32_t location)
        {
            if (m_InstancedLocations.empty())
                return false;
            auto size = m_InstancedLocations.size();
            if (size == 1)
                return m_InstancedLocations[0] <= location;
            for (int i = 0; i < size - 1; ++i)
            {
                if (location >= m_InstancedLocations[i] && location <= m_InstancedLocations[i + 1])
                    return true;
            }
            return false;
        }
        std::vector<BufferElement> m_Temp;
        std::vector<BufferElement> m_InElements;
        std::vector<BufferElement> m_InstancedElements;
        std::vector<uint32_t> m_InstancedLocations;
        std::vector<BufferUniformElement> m_UniformElements;
        std::vector<BufferElement> m_OutElements;
    };
} // namespace BeeEngine
