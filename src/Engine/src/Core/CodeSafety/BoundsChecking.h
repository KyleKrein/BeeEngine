//
// Created by Александр Лебедев on 22.06.2023.
//

#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include <version>
#ifdef __cpp_lib_source_location
#include <source_location>
#else
#include "Core/Logging/Log.h"
#include "source_location.h"

#endif

constexpr inline void
BeeAssertInBounds(auto&& x, auto&& arg, std::source_location location = std::source_location::current())
    requires(std::is_integral_v<std::remove_cvref_t<decltype(arg)>> && requires { std::ssize(x); })
{
    if consteval
    {
        static_assert(arg >= 0 && arg < std::ssize(x), "Index out of bounds");
        return;
    }
    if (arg < 0 || arg >= std::ssize(x))
    {
        std::string message =
            "Index out of bounds: " + std::to_string(arg) + " not in [0, " + std::to_string(std::ssize(x)) + ")";
        BeeLogError(BeeFormat("Index out of bounds: {1} not in [0, {2})", std::to_string(arg), std::ssize(x)));
    }
}

constexpr inline void
BeeAssertInBounds(auto&& x, auto&& arg, std::source_location location = std::source_location::current())
    requires(!(std::is_integral_v<std::remove_cvref_t<decltype(arg)>> && requires { std::ssize(x); }))
{
    if consteval
    {
        static_assert(arg >= 0 && arg < std::ssize(x), "Index out of bounds");
        return;
    }
    if (arg < 0 || arg >= std::ssize(x))
    {
        std::string message =
            "Index out of bounds: " + std::to_string(arg) + " not in [0, " + std::to_string(std::ssize(x)) + ")";
        BeeLogError(BeeFormat("Index out of bounds: {} not in [0, {})", std::to_string(arg), std::ssize(x)));
    }
}
