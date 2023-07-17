//
// Created by alexl on 12.07.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include "BufferLayout.h"

namespace BeeEngine
{
    namespace Internal
    {
        class RenderingQueue;
    }
    class InstancedBuffer
    {
    public:
        InstancedBuffer() = default;
        virtual ~InstancedBuffer() = default;
        InstancedBuffer(const InstancedBuffer& other) = delete;
        InstancedBuffer& operator=(const InstancedBuffer& other ) = delete;
        virtual void SetData(void* data, size_t size) = 0;
        virtual void Bind(void* cmd) = 0;
        virtual size_t GetSize() = 0;

        static Scope<InstancedBuffer> Create(size_t size);

        bool IsSubmitted() const { return m_IsSubmitted; }
    private:
        friend class ::BeeEngine::Internal::RenderingQueue;
        bool m_IsSubmitted = false;
        void Submit() { m_IsSubmitted = true; }
        void ResetSubmition() { m_IsSubmitted = false; }
        //virtual size_t GetMaxInstances() = 0;
        //virtual size_t GetOneInstanceSize() = 0;
        //virtual size_t GetSize() = 0;
        //virtual const std::vector<const BufferElement>& GetLayout() = 0;
    };
}