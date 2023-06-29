//
// Created by alexl on 14.05.2023.
//

#if 0
#pragma once
#include <cstddef>
#include <utility>

namespace BeeEngine
{
    template<typename T>
    class SharedPointer
    {
    public:
        SharedPointer() : m_ptr(nullptr), m_refCount(nullptr), m_Size(nullptr) {}

        SharedPointer(T* ptr) : m_ptr(ptr), m_refCount(new size_t(1)), m_Size(new size_t(sizeof(T))){}

        SharedPointer(const SharedPointer& other) : m_ptr(other.m_ptr), m_refCount(other.m_refCount), m_Size(other.m_Size) { ++*m_refCount; }

        SharedPointer(SharedPointer&& other) noexcept : m_ptr(other.m_ptr), m_Size(other.m_Size), m_refCount(other.m_refCount)
        {
            other.m_ptr = nullptr;
            other.m_refCount = nullptr;
        }

        SharedPointer& operator=(const SharedPointer& other)
        {
            if (this != &other)
            {
                Release();
                m_ptr = other.m_ptr;
                m_refCount = other.m_refCount;
                m_Size = other.m_Size;
                ++*m_refCount;
            }
            return *this;
        }

        SharedPointer& operator=(SharedPointer&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                m_ptr = other.m_ptr;
                m_refCount = other.m_refCount;
                m_Size = other.m_Size;
                other.m_ptr = nullptr;
                other.m_refCount = nullptr;
                other.m_Size = nullptr;
            }
            return *this;
        }

        ~SharedPointer() { Release(); }

        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr; }
        T* Get() const { return m_ptr; }

        explicit operator bool() const { return m_ptr != nullptr; }
        bool operator==(const SharedPointer& other) const { return m_ptr == other.m_ptr; }
        bool operator!=(const SharedPointer& other) const { return m_ptr != other.m_ptr; }


        // Конвертация в SharedPtr для базового класса
        template<typename U>
        operator SharedPointer<U>()
        {
            SharedPointer<U> sp(dynamic_cast<U*>(m_ptr), m_refCount, m_Size);
            (*m_refCount)++;
            return sp;
        }
        SharedPointer(void* ptr, size_t* refCount, size_t* size) : m_ptr((T*)ptr), m_refCount(refCount), m_Size(size) {}
    private:

        void Release()
        {
            if (m_ptr)
            {
                --*m_refCount;
                if (*m_refCount == 0)
                {
                    delete m_refCount;
                    ::operator delete(m_ptr, *m_Size);
                    delete m_Size;
                }
                m_ptr = nullptr;
                m_refCount = nullptr;
            }
        }

        T* m_ptr;
        size_t* m_refCount;
        size_t* m_Size;
    };

    template<typename T, typename ... Args>
    inline SharedPointer<T> MakeShared(Args&& ... args)
    {
        return SharedPointer<T>(new T(std::forward<Args>(args)...));
    }
}
#endif