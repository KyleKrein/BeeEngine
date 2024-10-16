//
// Created by alexl on 11.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "IBindable.h"

namespace BeeEngine
{
    class UniformBuffer : public IBindable
    {
    public:
        UniformBuffer() = default;
        ~UniformBuffer() override = default;
        UniformBuffer(const UniformBuffer& other) = delete;
        UniformBuffer& operator=(const UniformBuffer& other) = delete;
        static Scope<UniformBuffer> Create(size_t size);
        virtual void SetData(const void* data, size_t size) = 0;
    };
} // namespace BeeEngine
