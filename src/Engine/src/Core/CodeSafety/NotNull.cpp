#include "Expects.h"

namespace Bee
{
    template <typename T>
    class NotNull
    {
        public:
        constexpr NotNull(T* ptr)
        : m_Ptr(ptr)
        {
            if consteval
            {
                static_assert(ptr != nullptr, "Pointer cannot be null");
            }
            BeeExpects(ptr != nullptr);
        }
        constexpr NotNull& operator=(T* ptr)
        {
            if consteval
            {
                static_assert(ptr != nullptr, "Pointer cannot be null");
            }
            BeeExpects(ptr != nullptr);
            m_Ptr = ptr;
            return *this;
        }
        constexpr NotNull& operator=(const NotNull& other)
        {
            if consteval
            {
                static_assert(other.m_Ptr != nullptr, "Pointer cannot be null");
            }
            BeeExpects(other.m_Ptr != nullptr);
            m_Ptr = other.m_Ptr;
            return *this;
        }
        constexpr T* operator->() const
        {
            return m_Ptr;
        }
        constexpr T& operator*() const
        {
            return *m_Ptr;
        }
        constexpr operator T*() const
        {
            return m_Ptr;
        }
        constexpr operator T&() const
        {
            return *m_Ptr;
        }
        operator bool() const = delete;
        private:
        T* m_Ptr;
    };
}
