#include "Expects.h"
#include <algorithm>    // for forward
#include <cstddef>      // for ptrdiff_t, nullptr_t, size_t
#include <memory>       // for shared_ptr, unique_ptr
#include <system_error> // for hash
#include <type_traits>  // for enable_if_t, is_convertible, is_assignable

namespace Bee
{
    namespace Internal
    {
        template <typename T, typename = void>
        struct is_comparable_to_nullptr : std::false_type
        {
        };

        template <typename T>
        struct is_comparable_to_nullptr<
            T,
            std::enable_if_t<std::is_convertible<decltype(std::declval<T>() != nullptr), bool>::value>> : std::true_type
        {
        };
        template <typename T>
        using value_or_reference_return_t =
            std::conditional_t<sizeof(T) < 2 * sizeof(void*) && std::is_trivially_copy_constructible<T>::value,
                               const T,
                               const T&>;
    } // namespace Internal

    template <class T, class = std::enable_if_t<std::is_pointer<T>::value>>
    using owner = T;

    template <class T>
    class NotNull
    {
    public:
        static_assert(Internal::is_comparable_to_nullptr<T>::value, "T cannot be compared to nullptr.");

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr NotNull(U&& u) : m_Ptr(std::forward<U>(u))
        {
            BeeExpects(m_Ptr != nullptr);
        }

        template <typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
        constexpr NotNull(T u) : m_Ptr(std::move(u))
        {
            BeeExpects(m_Ptr != nullptr);
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr NotNull(const NotNull<U>& other) : NotNull(other.get())
        {
        }
        NotNull(const NotNull& other) = default;
        NotNull& operator=(const NotNull& other) = default;
        constexpr Internal::value_or_reference_return_t<T> get() const { return m_Ptr; }

        constexpr operator T() const { return get(); }
        constexpr decltype(auto) operator->() const { return get(); }
        constexpr decltype(auto) operator*() const { return *get(); }

        // prevents compilation when someone attempts to assign a null pointer constant
        NotNull(std::nullptr_t) = delete;
        NotNull& operator=(std::nullptr_t) = delete;

        // unwanted operators...pointers only point to single objects!
        NotNull& operator++() = delete;
        NotNull& operator--() = delete;
        NotNull operator++(int) = delete;
        NotNull operator--(int) = delete;
        NotNull& operator+=(std::ptrdiff_t) = delete;
        NotNull& operator-=(std::ptrdiff_t) = delete;
        void operator[](std::ptrdiff_t) const = delete;

    private:
        T* m_Ptr;
    };

    template <class T>
    std::ostream& operator<<(std::ostream& os, const NotNull<T>& val)
    {
        os << val.get();
        return os;
    }

    template <class T, class U>
    auto operator==(const NotNull<T>& lhs,
                    const NotNull<U>& rhs) noexcept(noexcept(lhs.get() == rhs.get())) -> decltype(lhs.get() ==
                                                                                                  rhs.get())
    {
        return lhs.get() == rhs.get();
    }

    template <class T, class U>
    auto operator!=(const NotNull<T>& lhs,
                    const NotNull<U>& rhs) noexcept(noexcept(lhs.get() != rhs.get())) -> decltype(lhs.get() !=
                                                                                                  rhs.get())
    {
        return lhs.get() != rhs.get();
    }

    template <class T, class U>
    auto operator<(const NotNull<T>& lhs, const NotNull<U>& rhs) noexcept(noexcept(std::less<>{}(lhs.get(), rhs.get())))
        -> decltype(std::less<>{}(lhs.get(), rhs.get()))
    {
        return std::less<>{}(lhs.get(), rhs.get());
    }

    template <class T, class U>
    auto operator<=(const NotNull<T>& lhs, const NotNull<U>& rhs) noexcept(
        noexcept(std::less_equal<>{}(lhs.get(), rhs.get()))) -> decltype(std::less_equal<>{}(lhs.get(), rhs.get()))
    {
        return std::less_equal<>{}(lhs.get(), rhs.get());
    }

    template <class T, class U>
    auto operator>(const NotNull<T>& lhs, const NotNull<U>& rhs) noexcept(
        noexcept(std::greater<>{}(lhs.get(), rhs.get()))) -> decltype(std::greater<>{}(lhs.get(), rhs.get()))
    {
        return std::greater<>{}(lhs.get(), rhs.get());
    }

    template <class T, class U>
    auto operator>=(const NotNull<T>& lhs, const NotNull<U>& rhs) noexcept(noexcept(
        std::greater_equal<>{}(lhs.get(), rhs.get()))) -> decltype(std::greater_equal<>{}(lhs.get(), rhs.get()))
    {
        return std::greater_equal<>{}(lhs.get(), rhs.get());
    }

    // more unwanted operators
    template <class T, class U>
    std::ptrdiff_t operator-(const NotNull<T>&, const NotNull<U>&) = delete;
    template <class T>
    NotNull<T> operator-(const NotNull<T>&, std::ptrdiff_t) = delete;
    template <class T>
    NotNull<T> operator+(const NotNull<T>&, std::ptrdiff_t) = delete;
    template <class T>
    NotNull<T> operator+(std::ptrdiff_t, const NotNull<T>&) = delete;

    template <class T,
              class U = decltype(std::declval<const T&>().get()),
              bool = std::is_default_constructible<std::hash<U>>::value>
    struct not_null_hash
    {
        std::size_t operator()(const T& value) const { return std::hash<U>{}(value.get()); }
    };

    template <class T, class U>
    struct not_null_hash<T, U, false>
    {
        not_null_hash() = delete;
        not_null_hash(const not_null_hash&) = delete;
        not_null_hash& operator=(const not_null_hash&) = delete;
    };
} // namespace Bee

namespace std
{
    template <class T>
    struct hash<Bee::NotNull<T>> : Bee::not_null_hash<Bee::NotNull<T>>
    {
    };

} // namespace std

namespace Bee
{
    //
    // strict_not_null
    //
    // Restricts a pointer or smart pointer to only hold non-null values,
    //
    // - provides a strict (i.e. explicit constructor from T) wrapper of not_null
    // - to be used for new code that wishes the design to be cleaner and make not_null
    //   checks intentional, or in old code that would like to make the transition.
    //
    //   To make the transition from not_null, incrementally replace not_null
    //   by strict_not_null and fix compilation errors
    //
    //   Expect to
    //   - remove all unneeded conversions from raw pointer to not_null and back
    //   - make API clear by specifying not_null in parameters where needed
    //   - remove unnecessary asserts
    //
    template <class T>
    class StrictNotNull : public NotNull<T>
    {
    public:
        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr explicit StrictNotNull(U&& u) : NotNull<T>(std::forward<U>(u))
        {
        }

        template <typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
        constexpr explicit StrictNotNull(T u) : NotNull<T>(u)
        {
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr StrictNotNull(const NotNull<U>& other) : NotNull<T>(other)
        {
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr StrictNotNull(const StrictNotNull<U>& other) : NotNull<T>(other)
        {
        }

        // To avoid invalidating the "not null" invariant, the contained pointer is actually copied
        // instead of moved. If it is a custom pointer, its constructor could in theory throw exceptions.
        StrictNotNull(StrictNotNull&& other) noexcept(std::is_nothrow_copy_constructible<T>::value) = default;
        StrictNotNull(const StrictNotNull& other) = default;
        StrictNotNull& operator=(const StrictNotNull& other) = default;
        StrictNotNull& operator=(const NotNull<T>& other)
        {
            NotNull<T>::operator=(other);
            return *this;
        }

        // prevents compilation when someone attempts to assign a null pointer constant
        StrictNotNull(std::nullptr_t) = delete;
        StrictNotNull& operator=(std::nullptr_t) = delete;

        // unwanted operators...pointers only point to single objects!
        StrictNotNull& operator++() = delete;
        StrictNotNull& operator--() = delete;
        StrictNotNull operator++(int) = delete;
        StrictNotNull operator--(int) = delete;
        StrictNotNull& operator+=(std::ptrdiff_t) = delete;
        StrictNotNull& operator-=(std::ptrdiff_t) = delete;
        void operator[](std::ptrdiff_t) const = delete;
    };

    // more unwanted operators
    template <class T, class U>
    std::ptrdiff_t operator-(const StrictNotNull<T>&, const StrictNotNull<U>&) = delete;
    template <class T>
    StrictNotNull<T> operator-(const StrictNotNull<T>&, std::ptrdiff_t) = delete;
    template <class T>
    StrictNotNull<T> operator+(const StrictNotNull<T>&, std::ptrdiff_t) = delete;
    template <class T>
    StrictNotNull<T> operator+(std::ptrdiff_t, const StrictNotNull<T>&) = delete;

    template <class T>
    auto make_strict_not_null(T&& t) noexcept
    {
        return StrictNotNull<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(t)};
    }

#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

    // deduction guides to prevent the ctad-maybe-unsupported warning
    template <class T>
    NotNull(T) -> NotNull<T>;
    template <class T>
    StrictNotNull(T) -> StrictNotNull<T>;

#endif // ( defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L) )

} // namespace Bee

namespace std
{
    template <class T>
    struct hash<Bee::StrictNotNull<T>> : Bee::not_null_hash<Bee::StrictNotNull<T>>
    {
    };

} // namespace std
