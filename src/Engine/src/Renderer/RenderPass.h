//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once
#include "CommandBuffer.h"
namespace BeeEngine
{
    using RenderPass = CommandBuffer;
    /*class RenderPass
    {
    public:
        RenderPass() = default;
        RenderPass(void* handle): m_Handle(handle) { }
        void* GetHandle() const { return m_Handle; }
        template<typename T>
        T GetHandleAs() const { return *static_cast<T*>(&m_Handle); }
        virtual ~RenderPass() = default;
    private:
        void* m_Handle = nullptr;
    };*/
} // namespace BeeEngine
