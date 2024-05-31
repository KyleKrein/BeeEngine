#pragma once
#include <cctype>
#include <type_traits>

namespace BeeEngine::Internal
{
    template <typename DecltypeT, bool IsIdExpression, typename T>
    constexpr std::remove_reference_t<T>&& Move_Impl(T&& t)
    {
        static_assert(!std::is_const_v<std::remove_reference_t<T>>, "Can't move const object");
        static_assert(!std::is_lvalue_reference_v<DecltypeT>, "Can't move from lvalue reference. It's unsafe.");
        static_assert(IsIdExpression, "Can't move from non-id expression.");
        static_assert(!std::is_trivial_v<std::remove_reference_t<T>>, "Can't move trivial object");
        static_assert(std::is_move_constructible_v<std::remove_reference_t<T>> ||
                          std::is_move_assignable_v<std::remove_reference_t<T>>,
                      "Can't move non-movable object");
        return static_cast<std::remove_reference_t<T>&&>(t);
    }
    constexpr bool IsAscii(int c)
    {
        return ((c & ~0x7F) == 0);
    }
    constexpr bool IsAlphabetAscii(char32_t c)
    {
        return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
    }
    constexpr bool IsDigitAscii(char32_t c)
    {
        return c >= '0' && c <= '9';
    }
    constexpr bool IsIdExpression(const char* expr)
    {
        for (auto str = expr; *str; ++str)
        {
            if (!IsAlphabetAscii(*str) && !IsDigitAscii(*str) && *str != '_' && *str != ':')
                return false;
        }
        return true;
    }
    template <typename DecltypeT, bool IsIdExpression, typename T>
    constexpr std::remove_reference_t<T>&& MoveIfOwned_Impl(T&& obj)
    {
        // DecltypeT is U >> U&&
        // DecltypeT is U& >> U&
        // DecltypeT is U&& >> U&&
        static_assert(IsIdExpression, "Can't move from lvalue reference. It's unsafe.");
        return static_cast<DecltypeT&&>(obj);
    }
} // namespace BeeEngine::Internal

namespace BeeEngine
{
    /**
     * @brief Moves always, even if the object is not const.
     * More dangerous. Use BeeMove instead.
     *
     * @tparam T
     * @param t
     * @return constexpr std::remove_reference_t<T>&&
     */
    template <typename T>
    constexpr std::remove_reference_t<T>&& MoveAlways(T&& t)
    {
        static_assert(!std::is_const_v<std::remove_reference_t<T>>, "Can't move const object");
        return static_cast<std::remove_reference_t<T>&&>(t);
    }

} // namespace BeeEngine
// Owning is moved. Non-owning is error. Argument must be id expression. Use instead of std::move
#define BeeMove(x) ::BeeEngine::Internal::Move_Impl<decltype(x), ::BeeEngine::Internal::IsIdExpression(#x)>(x)
// Owning is moved. Non-owning is copied. Argument must be id expression. Use instead of std::forward
#define BeeMoveIfOwned(x)                                                                                              \
    ::BeeEngine::Internal::MoveIfOwned_Impl<decltype(x), ::BeeEngine::Internal::IsIdExpression(#x)>(x)
// Owning is moved. Non-owning is moved. Argument can be anything. Prefer using BeeMove instead.
#define BeeMoveAlways(x) ::BeeEngine::MoveAlways(x)