//
// Created by alexl on 11.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
namespace BeeEngine
{
    class UniformBuffer
    {
    public:
        UniformBuffer() = default;
        virtual ~UniformBuffer() = default;
        UniformBuffer(const UniformBuffer& other) = delete;
        UniformBuffer& operator=(const UniformBuffer& other ) = delete;
        static Ref<UniformBuffer> Create(size_t size);
        virtual void SetData(void* data, size_t size) = 0;
        virtual void Bind(uint32_t binding) = 0;
    };
}
