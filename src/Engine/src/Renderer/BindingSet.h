//
// Created by alexl on 15.07.2023.
//

#pragma once
#include "Core/Move.h"
#include "Core/TypeDefines.h"
#include <vector>

namespace BeeEngine
{
    class Pipeline;
    class CommandBuffer;

    /**
     * @struct BindingSetElement
     * @brief Represents an element of a binding set, which consists of a binding point and associated data.
     */
    struct BindingSetElement
    {
        uint32_t Binding;      ///< The binding point in the shader or pipeline.
        class IBindable& Data; ///< Reference to the data object to be bound.
    };

    /**
     * @class BindingSet
     * @brief Represents a set of bindings that can be applied to a pipeline.
     *
     * A BindingSet is responsible for managing a collection of bindings, which
     * map data to specific binding points in a pipeline. The bindings are stored
     * internally and can be applied to a command buffer in the context of a pipeline.
     */
    class BindingSet
    {
    public:
        /**
         * @brief Constructs a BindingSet with a given set of elements.
         * @param elements A vector of BindingSetElement, each representing a binding point and its associated data.
         */
        BindingSet(std::vector<BindingSetElement> elements) : m_Elements(BeeMove(elements)) {}

        /**
         * @brief Binds the elements of the BindingSet to the specified pipeline.
         * @param cmd The command buffer where the bindings will be applied.
         * @param index The index in the binding set array to bind.
         * @param pipeline The pipeline to which the bindings will be applied.
         */
        virtual void Bind(CommandBuffer& cmd, uint32_t index, Pipeline& pipeline) const = 0;

        /**
         * @brief Virtual destructor to ensure proper cleanup in derived classes.
         */
        virtual ~BindingSet() = default;

        /**
         * @brief Creates a BindingSet with a list of elements.
         * @param elements An initializer list of BindingSetElement.
         * @return A scoped/unique pointer (Scope) to the created BindingSet.
         */
        static Scope<BindingSet> Create(std::initializer_list<BindingSetElement> elements);

        /**
         * @brief Creates a BindingSet with a vector of elements.
         * @param elements A vector of BindingSetElement.
         * @return A scoped/unique pointer (Scope) to the created BindingSet.
         */
        static Scope<BindingSet> Create(std::vector<BindingSetElement> elements);

        /**
         * @brief Creates a BindingSet with a list of elements, with a lifetime tied to a frame.
         * @param elements An initializer list of BindingSetElement.
         * @return A frame-scoped pointer (FrameScope) to the created BindingSet.
         */
        static FrameScope<BindingSet> CreateFrameScope(std::initializer_list<BindingSetElement> elements);

    protected:
        std::vector<BindingSetElement> m_Elements; ///< The collection of binding elements stored in this BindingSet.
    };
} // namespace BeeEngine
