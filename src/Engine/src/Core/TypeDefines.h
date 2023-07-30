#pragma once
#include <string>
#include <memory>
#include "SharedPointer.h"
#include "Core/CodeSafety/Expects.h"
#include "FramePtr.h"
#include <vector>
#include <optional>
#include <concepts>
#if __has_include (<cxxabi.h>)
#include <cxxabi.h>
#define BEE_HAS_CXXABI_H
#define CONSTEXPR_FUNC constexpr
#else
#define CONSTEXPR_FUNC inline
#endif

#include <magic_enum.hpp>
#if defined(BEE_COMPILE_WEBGPU)
#include <webgpu/webgpu.h>
#endif


namespace BeeEngine
{
    using String = std::string;

    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T>
    using Ref = std::shared_ptr<T>;

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
    template<typename T>
    CONSTEXPR_FUNC String ToString(const T& obj);

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
    CONSTEXPR_FUNC std::string ToString(const T& obj)
    {
        return obj.ToString();
    }

    template<typename T>
    requires std::is_enum_v<T>
    CONSTEXPR_FUNC String EnumToString(T obj);

    template<typename T>
    CONSTEXPR_FUNC String TypeName(const T& obj)
    {
#if defined(BEE_HAS_CXXABI_H)
        int status;
        char * demangled = abi::__cxa_demangle(typeid(obj).name(),0,0,&status);
        String result = String(demangled);
        free(demangled);
        return result;
#else
        return String(typeid(obj).name());
#endif
    }

    template<typename T>
    CONSTEXPR_FUNC String ToString(const T& obj)
    {
        if constexpr(std::is_enum_v<T>)
            return EnumToString(obj);
        return TypeName(obj);
    }
    template<typename T>
    requires std::is_enum_v<T>
    CONSTEXPR_FUNC String EnumToString(T obj)
    {
        //String name = String(magic_enum::enum_type_name<T>()) + "::" + String(magic_enum::enum_name(obj));
        auto name = magic_enum::enum_name(obj);
        if (name.empty())
        {
            return String{magic_enum::enum_type_name<T>()} + "::" +
                   String{std::to_string(static_cast<std::underlying_type_t<T>>(obj))};
        } else
            return String{magic_enum::enum_name(obj)};
    }
#if defined(BEE_COMPILE_WEBGPU)
    template<>
    CONSTEXPR_FUNC String EnumToString<WGPUFeatureName>(WGPUFeatureName obj)
    {
        switch (obj)
        {
            case WGPUFeatureName_Undefined:
                return "Undefined";
                break;
            case WGPUFeatureName_DepthClipControl:
                return "DepthClipControl";
                break;
            case WGPUFeatureName_Depth32FloatStencil8:
                return "Depth32FloatStencil8";
                break;
            case WGPUFeatureName_TimestampQuery:
                return "TimestampQuery";
                break;
            case WGPUFeatureName_PipelineStatisticsQuery:
                return "PipelineStatisticsQuery";
                break;
            case WGPUFeatureName_TextureCompressionBC:
                return "TextureCompressionBC";
                break;
            case WGPUFeatureName_TextureCompressionETC2:
                return "TextureCompressionETC2";
                break;
            case WGPUFeatureName_TextureCompressionASTC:
                return "TextureCompressionASTC";
                break;
            case WGPUFeatureName_IndirectFirstInstance:
                return "IndirectFirstInstance";
                break;
            case WGPUFeatureName_ShaderF16:
                return "ShaderF16";
                break;
            case WGPUFeatureName_RG11B10UfloatRenderable:
                return "RG11B10UfloatRenderable";
                break;
            case WGPUFeatureName_BGRA8UnormStorage:
                return "BGRA8UnormStorage";
                break;
            case WGPUFeatureName_Float32Filterable:
                return "Float32Filterable";
                break;
            case WGPUFeatureName_DawnShaderFloat16:
                return "DawnShaderFloat16";
                break;
            case WGPUFeatureName_DawnInternalUsages:
                return "DawnInternalUsages";
                break;
            case WGPUFeatureName_DawnMultiPlanarFormats:
                return "DawnMultiPlanarFormats";
                break;
            case WGPUFeatureName_DawnNative:
                return "DawnNative";
                break;
            case WGPUFeatureName_ChromiumExperimentalDp4a:
                return "ChromiumExperimentalDp4a";
                break;
            case WGPUFeatureName_TimestampQueryInsidePasses:
                return "TimestampQueryInsidePasses";
                break;
            case WGPUFeatureName_ImplicitDeviceSynchronization:
                return "ImplicitDeviceSynchronization";
                break;
            case WGPUFeatureName_SurfaceCapabilities:
                return "SurfaceCapabilities";
                break;
            case WGPUFeatureName_TransientAttachments:
                return "TransientAttachments";
                break;
            case WGPUFeatureName_MSAARenderToSingleSampled:
                return "MSAARenderToSingleSampled";
                break;
            case WGPUFeatureName_Force32:
                return "Force32";
                break;
            default:
                return "Unknown";
                break;
        }
    }
#endif


    template<>
    CONSTEXPR_FUNC String ToString<String>(const String& obj)
    {
        return obj;
    }
    template<>
    CONSTEXPR_FUNC String ToString<std::string_view>(const std::string_view& obj)
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
    CONSTEXPR_FUNC String ToString<int>(const int& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<unsigned int>(const unsigned int&obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<long>(const long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<unsigned long>(const unsigned long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<long long>(const long long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<unsigned long long>(const unsigned long long& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<float>(const float& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<double>(const double& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<long double>(const long double& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<bool>(const bool& obj)
    {
        return obj ? "true" : "false";
    }
    template<>
    CONSTEXPR_FUNC String ToString<char>(const char& obj)
    {
        return String{obj};
    }
    template<>
    CONSTEXPR_FUNC String ToString<unsigned char>(const unsigned char& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<signed char>(const signed char& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<wchar_t>(const wchar_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<char8_t>(const char8_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<char16_t>(const char16_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<char32_t>(const char32_t& obj)
    {
        return std::to_string(obj);
    }
    template<>
    CONSTEXPR_FUNC String ToString<std::nullptr_t>(const std::nullptr_t& obj)
    {
        return obj == nullptr ? "nullptr" : std::to_string(reinterpret_cast<std::uintptr_t>(obj));
    }
    template<typename T>
    requires std::is_pointer_v<T>
    CONSTEXPR_FUNC String ToString(T obj)
    {
        return obj == nullptr ? "nullptr" : std::to_string(reinterpret_cast<std::uintptr_t>(obj));
    }
    template<>
    CONSTEXPR_FUNC String ToString<std::byte>(const std::byte& obj)
    {
        return std::to_string(static_cast<int>(obj));
    }

    template<typename T>
    CONSTEXPR_FUNC String ToString(const std::vector<T>& collection)
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
    CONSTEXPR_FUNC String ToString(const std::array<T, numberOfElements>& collection)
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
    CONSTEXPR_FUNC String ToString(const std::unordered_map<Key, Value>& collection)
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
#undef CONSTEXPR_FUNC