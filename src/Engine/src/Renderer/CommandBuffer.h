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
    protected:
        void* m_Handle;
    };
}
