//
// Created by alexl on 15.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
namespace BeeEngine
{
    class Pipeline;
    class CommandBuffer;
    struct BindingSetElement
    {
        uint32_t Binding;
        class IBindable& Data;
    };
    class BindingSet
    {
    public:
        BindingSet(std::initializer_list<BindingSetElement> elements) : m_Elements(elements) {}
        virtual void Bind(CommandBuffer& cmd, uint32_t index, Pipeline& pipeline) const = 0;
        virtual ~BindingSet() = default;
        static Ref<BindingSet> Create(std::initializer_list<BindingSetElement> elements);
        static FrameScope<BindingSet> CreateFrameScope(std::initializer_list<BindingSetElement> elements);

    protected:
        std::vector<BindingSetElement> m_Elements;
    };
} // namespace BeeEngine
