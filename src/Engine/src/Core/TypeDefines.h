#pragma once
#include <string>
#include <memory>
#include "SharedPointer.h"
#include <vector>

#define USE_CUSTOM_CONTAINERS 0

namespace BeeEngine
{
    using String = std::string;

    template<typename T>
    using Scope = std::unique_ptr<T>;

#if USE_CUSTOM_CONTAINERS
    template<typename T>
    using Ref = SharedPointer<T>;
#else
    template<typename T>
    using Ref = std::shared_ptr<T>;
#endif

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

    template<typename T>
    using List = std::vector<T>;
}