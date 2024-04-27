#pragma once
#include <string>
#include <memory>
#include "SharedPointer.h"
#include "Core/CodeSafety/Expects.h"
#include "FramePtr.h"
#include <vector>
#include <optional>
#include <concepts>
#include "ToString.h"

#include "String.h"
#include "Numbers.h"

namespace BeeEngine
{
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T>
    using WeakRef = std::weak_ptr<T>;

    template<typename T>
    using FrameScope = FramePtr<T>;

    template<typename T, typename ...Args>
    constexpr FrameScope<T> CreateFrameScope(Args&& ...args)
    {
        T* ptr = new T(std::forward<Args>(args)...);
        return FramePtr<T>(ptr);
    }

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
#if USE_CUSTOM_CONTAINERS
        return MakeShared<T>(args...);
#else
        return std::make_shared<T>(std::forward<Args>(args)...);
#endif
    }
    namespace Internal
    {
        template<typename T>
        constexpr bool prefer_pass_by_value =
                sizeof(T) <= 2*sizeof(void*)
                && std::is_trivially_copy_constructible_v<T>;

        template<typename T>
        requires std::is_class_v<T> || std::is_union_v<T> || std::is_array_v<T> || std::is_function_v<T>
        constexpr bool prefer_pass_by_value<T> = false;
    }
    template<typename T>
            requires (!std::is_void_v<T>)
    using ConstGet =
            std::conditional_t <
                    Internal::prefer_pass_by_value<T>,
                    T const,
                    T const&
            >;

    using byte = std::byte;
    template<typename T>
            requires (!std::is_void_v<T>)
    using in =
            std::conditional_t <
                    Internal::prefer_pass_by_value<T>,
                    T const,
                    T const&
            >;

    template<typename T>
            requires (!std::is_void_v<T>)
    using out = T&;

    template<typename T>
    using List = std::vector<T>;
}
