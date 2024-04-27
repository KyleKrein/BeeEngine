//
// Created by Александр Лебедев on 16.10.2023.
//
#include "Core/String.h"
#include <vector>
#include <cstddef>
#pragma once
namespace BeeEngine::Reflection
{
    struct TypeDescriptor
    {
        const char *Name;
        size_t Size;

        TypeDescriptor(const char *name, size_t size) : Name{name}, Size{size}
        {}

        virtual ~TypeDescriptor()
        {}

        virtual String GetFullName() const
        { return Name; }
    };

    struct MethodDescriptor
    {
        const char *Name;
        //void *Pointer;
        //TypeDescriptor *ReturnType;
        //std::vector<TypeDescriptor *> Arguments;
    };

    // Declare the function template that handles primitive types such as int, std::string, etc.:
    template<typename T>
    TypeDescriptor &GetPrimitiveDescriptor();

    template <typename T>
    requires std::is_pointer_v<T>
    TypeDescriptor& GetPrimitiveDescriptor() {
        static TypeDescriptor typeDesc{"void*", sizeof(T)};
        return typeDesc;
    }
    struct TypeDescriptor_Struct;
    // A helper class to find TypeDescriptors in different ways:
    struct DefaultResolver
    {
        template<typename T>
        static char func(decltype(&T::Reflection));

        template<typename T>
        static int func(...);

        template<typename T>
        struct IsReflected
        {
            enum
            {
                value = (sizeof(func<T>(nullptr)) == sizeof(char))
            };
        };
        // This version is called if T has a static member named "Reflection":
        template<typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
        static TypeDescriptor_Struct &Get()
        {
            return T::Reflection;
        }

        // This version is called otherwise:
        template<typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
        static TypeDescriptor &Get()
        {
            return GetPrimitiveDescriptor<T>();
        }
    };
    template<class T>
    concept HasReflection = requires
    {
        T::Reflection;
    };
    // This is the primary class template for finding all TypeDescriptors:
    struct TypeResolver
    {

        template<HasReflection T>
        static TypeDescriptor_Struct &Get()
        {
            return DefaultResolver::Get<T>();
        }
        template<typename T>
        static TypeDescriptor &Get()
        {
            return DefaultResolver::Get<T>();
        }
    };

    struct TypeDescriptor_Struct : TypeDescriptor
    {
        struct Member
        {
            const char *Name;
            size_t Offset;
            TypeDescriptor *Type;

            void* GetPtr(void* objectPtr) const
            {
                return static_cast<char*>(objectPtr) + Offset;
            }

            template<typename T, typename StructType>
            T& Get(StructType& object) const
            {
                return *static_cast<T*>(GetPtr(&object));
            }
        };

        std::vector<Member> Members;
        std::vector<MethodDescriptor> Methods;

        TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct *)

        ) : TypeDescriptor{ nullptr, 0 }
        {
            init(this);
        }

        TypeDescriptor_Struct(const char *name, size_t size, const std::initializer_list<Member> &members,
                              const std::initializer_list<MethodDescriptor> &methods)
                : TypeDescriptor{name, size}, Members{members}, Methods{methods}
        {}
    };


#define REFLECT() \
    friend struct ::BeeEngine::Reflection::DefaultResolver; \
    static ::BeeEngine::Reflection::TypeDescriptor_Struct Reflection; \
    static void InitReflection(::BeeEngine::Reflection::TypeDescriptor_Struct*);

#define REFLECT_STRUCT_BEGIN(type) \
    ::BeeEngine::Reflection::TypeDescriptor_Struct type::Reflection{type::InitReflection}; \
    void type::InitReflection(::BeeEngine::Reflection::TypeDescriptor_Struct* typeDesc) { \
        using T = type; \
        typeDesc->Name = #type; \
        typeDesc->Size = sizeof(T); \
        typeDesc->Members = {

#define REFLECT_STRUCT_MEMBER(name) \
            ::BeeEngine::Reflection::TypeDescriptor_Struct::Member{#name, offsetof(T, name), &::BeeEngine::Reflection::TypeResolver::Get<decltype(T::name)>()},
/*
#define REFLECT_MEMBERS_END() \
        };
#define REFLECT_METHODS_BEGIN() \
        typeDesc->Methods = {

#define REFLECT_METHOD(name) \
            ::BeeEngine::Reflection::MethodDescriptor{#name},


            */
#define REFLECT_STRUCT_END() \
        };                    \
    }

#define REFLECT_ENUM(name) \
    template <> \
    ::BeeEngine::Reflection::TypeDescriptor& ::BeeEngine::Reflection::GetPrimitiveDescriptor<name>() { \
        static TypeDescriptor typeDesc{#name, sizeof(name)}; \
        return typeDesc; \
    }
}