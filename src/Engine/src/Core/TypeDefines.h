#pragma once
#include <string>
#include <memory>
#include "SharedPointer.h"

namespace BeeEngine
{
    using String = std::string;

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = SharedPointer<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return MakeShared<T>(args...);
        //return std::make_shared<T>(std::forward<Args>(args)...);
    }
}