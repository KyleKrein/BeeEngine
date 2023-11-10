//
// Created by alexl on 06.11.2023.
//

#pragma once
#include <utility>
namespace BeeEngine
{
    template<typename T, typename U>
    constexpr T narrow_cast(U&& u) noexcept
    {
        return static_cast<T>(std::forward<U>(u));
    }
}