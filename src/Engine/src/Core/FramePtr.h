//
// Created by alexl on 19.07.2023.
//

#pragma once
#include "Core/DeletionQueue.h"
#include "Logging/Log.h"
#include <vector>
namespace BeeEngine
{
    template <typename T>
    class FramePtr final
    {
    public:
        FramePtr() = default;
        FramePtr(T* ptr)
        {
            m_Ptr = ptr;
            DeletionQueue::Frame().PushFunction([ptr]() { delete ptr; });
        }

        // template<typename ...Args>
        // FramePtr(Args&& ...args);

        FramePtr(const FramePtr& other) { m_Ptr = other.m_Ptr; }
        FramePtr& operator=(const FramePtr& other)
        {
            m_Ptr = other.m_Ptr;
            return *this;
        }
        virtual ~FramePtr() = default;

        // Конвертация в FramePtr для базового класса
        template <typename U>
        operator FramePtr<U>()
        {
            return {dynamic_cast<U*>(m_Ptr), true};
        }

        // T* operator->() const;
        // T& operator*() const;
        // T* Get() const;
        T* Get() { return m_Ptr; }
        T* operator->()
        {
            BeeExpects(m_Ptr != nullptr);
            return m_Ptr;
        }
        T& operator*()
        {
            BeeExpects(m_Ptr != nullptr);
            return *m_Ptr;
        }

        // does not delete the pointer
        FramePtr(T* ptr, bool) : m_Ptr(ptr) {}

    private:
        T* m_Ptr{nullptr};
    };
} // namespace BeeEngine
