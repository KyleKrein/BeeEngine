//
// Created by alexl on 08.10.2023.
//

#pragma once
#include <string>
#include <ranges>
#include <vector>
#include <array>
#include <unordered_map>
#include "String.h"
#if __has_include (<cxxabi.h>)
#include <cxxabi.h>
#define BEE_HAS_CXXABI_H
#define CONSTEXPR_FUNC inline
#else
#define CONSTEXPR_FUNC inline
#endif

#include <magic_enum.hpp>
#if defined(BEE_COMPILE_WEBGPU)
#include <webgpu/webgpu.h>
#endif
#include "spdlog/spdlog.h"
namespace BeeEngine
{
    template<typename T>
    CONSTEXPR_FUNC String ToString(const T &obj);

    template<class T>
    concept ToStringAble =
    requires(T value)
    {
        (!std::is_enum_v<T>) && std::is_class_v<T> &&
        std::is_same_v<UTF8String, decltype(std::declval<T>().ToString())>;
    };

    template<ToStringAble T>
    CONSTEXPR_FUNC std::string ToString(const T &obj)
    {
        return obj.ToString();
    }

    CONSTEXPR_FUNC String ToString(const char *obj)
    {
        return String(obj);
    }

    CONSTEXPR_FUNC String ToString(char *obj)
    {
        return String(obj);
    }

    template<typename T>
    requires std::is_enum_v<T>
    constexpr String EnumToString(T obj);

    template<typename T>
    CONSTEXPR_FUNC String TypeName(const T &obj)
    {
#if defined(BEE_HAS_CXXABI_H)
        int status;
            char * demangled = abi::__cxa_demangle(typeid(obj).name(),0,0,&status);
            String result = String(demangled);
            free(demangled);
            return result;
#else
        std::string_view typeName = typeid(obj).name();
        size_t pos = typeName.find_first_of(" ");
        //if (pos != std::string_view::npos)
        return String(typeName.substr(pos + 1));
        //else
        //    return String(typeName);
#endif
    }

    template<typename T>
    CONSTEXPR_FUNC String TypeName()
    {
#if defined(BEE_HAS_CXXABI_H)
        int status;
            char * demangled = abi::__cxa_demangle(typeid(T).name(),0,0,&status);
            String result = String(demangled);
            free(demangled);
            return result;
#else
        std::string_view typeName = typeid(T).name();
        size_t pos = typeName.find_last_of(' ');
        //if (pos != std::string_view::npos)
        return String(typeName.substr(pos + 1));
        //else
        //    return String(typeName);
#endif
    }

    template<typename T>
    CONSTEXPR_FUNC String ToString(const T &obj)
    {
        if constexpr (std::is_enum_v<T>)
            return EnumToString(obj);
        return TypeName(obj);
    }

    template<typename T>
    requires std::is_enum_v<T>
    constexpr String EnumToString(T obj)
    {
        auto name = magic_enum::enum_name(obj);
        if (name.empty())
        {
            return String{magic_enum::enum_type_name<T>()} + "::" +
                   String{std::to_string(static_cast<std::underlying_type_t<T>>(obj))};
        } else
            return String{name};
    }

    template<typename T>
    requires std::is_enum_v<T>
    constexpr T StringToEnum(const String& str)
    {
        if(str.contains("::"))
        {
            auto parts = SplitString(str, "::");
            return static_cast<T>(std::stoi(parts[1].data()));
        }
        else
        {
            return magic_enum::enum_cast<T>(str).value();
        }
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
    CONSTEXPR_FUNC String ToString<String>(const String &obj)
    {
        return obj;
    }

    template<>
    CONSTEXPR_FUNC String ToString<std::string_view>(const std::string_view &obj)
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
    CONSTEXPR_FUNC String ToString<int>(const int &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<unsigned int>(const unsigned int &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<long>(const long &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<unsigned long>(const unsigned long &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<long long>(const long long &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<unsigned long long>(const unsigned long long &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<float>(const float &obj)
    {
        //return std::to_string(obj);
        return fmt::format("{:f}", obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<double>(const double &obj)
    {
        //return std::to_string(obj);
        return fmt::format("{:f}", obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<long double>(const long double &obj)
    {
        return fmt::format("{:f}", obj);//std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<bool>(const bool &obj)
    {
        return obj ? "true" : "false";
    }

    template<>
    CONSTEXPR_FUNC String ToString<char>(const char &obj)
    {
        return String{obj};
    }

    template<>
    CONSTEXPR_FUNC String ToString<unsigned char>(const unsigned char &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<signed char>(const signed char &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<wchar_t>(const wchar_t &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<char8_t>(const char8_t &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<char16_t>(const char16_t &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<char32_t>(const char32_t &obj)
    {
        return std::to_string(obj);
    }

    template<>
    CONSTEXPR_FUNC String ToString<std::nullptr_t>(const std::nullptr_t &obj)
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
    CONSTEXPR_FUNC String ToString<std::byte>(const std::byte &obj)
    {
        return std::to_string(static_cast<int>(obj));
    }
    template<std::ranges::range T>
    requires (!std::is_convertible_v<T, String>)
    CONSTEXPR_FUNC String ToString(const T& collection)
    {
        String result = "[";
        for (auto &item: collection)
        {
            result += ToString(item) + ", ";
        }
        result.replace(result.size() - 2, 2, "]");
        return result;
    }
    template<typename P1, typename P2>
    CONSTEXPR_FUNC String ToString(const std::pair<P1, P2> &obj)
    {
        return "[" + ToString(obj.first) + " : " + ToString(obj.second) + "]";
    }
}
#undef CONSTEXPR_FUNC