#pragma once
#include <string>
#include <memory>
#include "SharedPointer.h"
#include "Core/CodeSafety/Expects.h"
#include <vector>
#include <optional>
#include <cxxabi.h>
#include <magic_enum.hpp>

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
    //-----------------------------------------------------------------------
    //
    //  in<T>       For "in" parameter
    //
    //-----------------------------------------------------------------------
    //
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
/*
//-----------------------------------------------------------------------
//
//  Initialization: These are closely related...
//
//  deferred_init<T>    For deferred-initialized local object
//
//  out<T>              For out parameter
//
//-----------------------------------------------------------------------
//
    template<typename T>
    class deferred_init {
        bool init = false;
        alignas(T) std::byte data[sizeof(T)]; // or: std::aligned_storage_t<sizeof(T), alignof(T)> data

        auto t() -> T& { return *std::launder(reinterpret_cast<T*>(&data)); }

        template<typename U>
        friend class out;

        auto destroy() -> void         { if (init) { t().~T(); }  init = false; }

    public:
        deferred_init() noexcept       { }
        ~deferred_init() noexcept       { destroy(); }
        auto value()    noexcept -> T& { BeeExpects(init);  return t(); }

        auto construct     (auto&& ...args) -> void { BeeExpects(!init);  new (&data) T(std::forward<decltype(args)>(args)...);  init = true; }
        auto construct_list(auto&& ...args) -> void { BeeExpects(!init);  new (&data) T{std::forward<decltype(args)>(args)...};  init = true; }
    };


    template<typename T>
    class out {
        //  Not going to bother with std::variant here
        union {
            T* t;
            deferred_init<T>* dt;
        };
        out<T>* ot = {};
        bool has_t;
        inline auto Uncaught_exceptions() -> int {
#ifdef BEE_NO_EXCEPTIONS
            return 0;
#else
            return std::uncaught_exceptions();
#endif
        }

        //  Each out in a chain contains its own uncaught_count ...
        int  uncaught_count   = Uncaught_exceptions();
        //  ... but all in a chain share the topmost called_construct_
        bool called_construct_ = false;

    public:
        out(T*                 t_) noexcept :  t{ t_}, has_t{true}       { BeeExpects( t); }
        out(deferred_init<T>* dt_) noexcept : dt{dt_}, has_t{false}      { BeeExpects(dt); }
        out(out<T>*           ot_) noexcept : ot{ot_}, has_t{ot_->has_t} { BeeExpects(ot);
            if (has_t) {  t = ot->t;  }
            else       { dt = ot->dt; }
        }

        auto called_construct() -> bool& {
            if (ot) { return ot->called_construct(); }
            else    { return called_construct_; }
        }

        //  In the case of an exception, if the parameter was uninitialized
        //  then leave it in the same state on exit (strong guarantee)
        ~out() {
            if (called_construct() && uncaught_count != Uncaught_exceptions()) {
                BeeExpects(!has_t);
                dt->destroy();
                called_construct() = false;
            }
        }

        auto construct(auto&& ...args) -> void {
            if (has_t || called_construct()) {
                if constexpr (requires { *t = T(std::forward<decltype(args)>(args)...); }) {
                    BeeExpects( t );
                    *t = T(std::forward<decltype(args)>(args)...);
                }
                else {
                    BeeExpects(!"attempted to copy assign, but copy assignment is not available");
                }
            }
            else {
                BeeExpects( dt );
                if (dt->init) {
                    if constexpr (requires { *t = T(std::forward<decltype(args)>(args)...); }) {
                        dt->value() = T(std::forward<decltype(args)>(args)...);
                    }
                    else {
                        BeeExpects(!"attempted to copy assign, but copy assignment is not available");
                    }
                }
                else {
                    dt->construct(std::forward<decltype(args)>(args)...);
                    called_construct() = true;
                }
            }
        }

        auto construct_list(auto&& ...args) -> void {
            if (has_t || called_construct()) {
                if constexpr (requires { *t = T{std::forward<decltype(args)>(args)...}; }) {
                    BeeExpects( t );
                    *t = T{std::forward<decltype(args)>(args)...};
                }
                else {
                    BeeExpects(!"attempted to copy assign, but copy assignment is not available");
                }
            }
            else {
                BeeExpects( dt );
                if (dt->init) {
                    if constexpr (requires { *t = T{std::forward<decltype(args)>(args)...}; }) {
                        dt->value() = T{std::forward<decltype(args)>(args)...};
                    }
                    else {
                        BeeExpects(!"attempted to copy assign, but copy assignment is not available");
                    }
                }
                else {
                    dt->construct(std::forward<decltype(args)>(args)...);
                    called_construct() = true;
                }
            }
        }

        auto value() noexcept -> T& {
            if (has_t) {
                BeeExpects( t );
                return *t;
            }
            else {
                BeeExpects( dt );
                return dt->value();
            }
        }
    };
*/

    template<typename T>
    using List = std::vector<T>;
    template<typename T>
    constexpr String ToString(const T& obj);

    template<class T>
    concept ConceptToStringAble = requires (T value)
    {
        value.ToString() -> std::string;
    };

    template<typename T>
    bool IsToStringAble()
    {
        return std::is_same_v<std::string, decltype(std::declval<T>().ToString())>;
    }

    template<typename T>
    requires ConceptToStringAble<T>
    constexpr std::string ToString(const T& obj)
    {
        return obj.ToString();
    }

    template<typename T>
    requires std::is_enum_v<T>
    constexpr String EnumToString(T obj);

    template<typename T>
    constexpr String TypeName(const T& obj)
    {
        int status;
        char * demangled = abi::__cxa_demangle(typeid(obj).name(),0,0,&status);
        String result = String(demangled);
        free(demangled);
        return result;
    }

    template<typename T>
    constexpr String ToString(const T& obj)
    {
        if constexpr(std::is_enum_v<T>)
            return EnumToString(obj);
        return TypeName(obj);
    }
    template<typename T>
    requires std::is_enum_v<T>
    constexpr String EnumToString(T obj)
    {
        //String name = String(magic_enum::enum_type_name<T>()) + "::" + String(magic_enum::enum_name(obj));
        auto name = magic_enum::enum_name(obj);
        if(name.empty())
            return String{magic_enum::enum_type_name<T>()} + "::" + String{std::to_string(static_cast<std::underlying_type_t<T>>(obj))};
        else
            return String{magic_enum::enum_name(obj)};
    }


    template<>
    constexpr String ToString<String>(const String& obj)
    {
        return obj;
    }
    template<>
    constexpr String ToString<std::string_view>(const std::string_view& obj)
    {
        return String{obj};
    }
    /*
    template<>
    constexpr String ToString<char*>(const char*& obj)
    {
        return String{(const char*)&obj};
    }
    */
    template<>
    constexpr String ToString<int>(const int& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<unsigned int>(const unsigned int&obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<long>(const long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<unsigned long>(const unsigned long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<long long>(const long long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<unsigned long long>(const unsigned long long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<float>(const float& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<double>(const double& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<long double>(const long double& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<bool>(const bool& obj)
    {
        return obj ? "true" : "false";
    }
    template<>
    constexpr String ToString<char>(const char& obj)
    {
        return String{obj};
    }
    template<>
    constexpr String ToString<unsigned char>(const unsigned char& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<signed char>(const signed char& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<wchar_t>(const wchar_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<char8_t>(const char8_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<char16_t>(const char16_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<char32_t>(const char32_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    constexpr String ToString<std::nullptr_t>(const std::nullptr_t& obj)
    {
        return obj == nullptr ? "nullptr" : std::to_string(reinterpret_cast<std::uintptr_t>(obj));
    }
    template<typename T>
    requires std::is_pointer_v<T>
    constexpr String ToString(T obj)
    {
        return obj == nullptr ? "nullptr" : std::to_string(reinterpret_cast<std::uintptr_t>(obj));
    }
    template<>
    constexpr String ToString<std::byte>(const std::byte& obj)
    {
        return std::to_string(static_cast<int>(obj));
    }

    template<typename T>
    constexpr String ToString(const std::vector<T>& collection)
    {
        String result = "[";
        for(auto& item : collection)
        {
            result += ToString(item) + ", ";
        }
        result.replace(result.size() - 2, 2, "]");
        return result;
    }
    template<typename T, size_t numberOfElements>
    constexpr String ToString(const std::array<T, numberOfElements>& collection)
    {
        String result = "[";
        for(auto& item : collection)
        {
            result += ToString(item) + ", ";
        }
        result.replace(result.size() - 2, 2, "]");
        return result;
    }
    template<typename Key, typename Value>
    constexpr String ToString(const std::unordered_map<Key, Value>& collection)
    {
        String result = "[";
        for(auto& item : collection)
        {
            result += "[" + ToString(item.first) + " : " + ToString(item.second) + "]" ", ";
        }
        result.replace(result.size() - 2, 2, "]");
        return result;
    }
}