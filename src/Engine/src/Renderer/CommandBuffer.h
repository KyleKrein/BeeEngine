//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once

namespace BeeEngine
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(void* handle)
            : m_Handle(handle)
        {}
        virtual ~CommandBuffer() = default;

        operator bool()
        {
            return m_Handle != nullptr;
        }
        void* GetHandle() const
        {
            return m_Handle;
        }
        template<typename T>
        T GetHandleAs() const
        {
            return *reinterpret_cast<T const *>(&m_Handle);
        }
    protected:
        void* m_Handle;
    };
}
