//
// Created by alexl on 17.10.2023.
//
#include <cinttypes>
#include <numeric>
#include "String.h"
#include "TypeSequence.h"
#include "Reflection.h"

#pragma once
namespace BeeEngine
{
    class HashAlgorithm
    {
    public:
        static uint32_t MurmurHash2(const void * key, size_t len, uint32_t seed);
        static uint32_t MurmurHash2(const UTF8String & key, uint32_t seed);
        static uint32_t MurmurHash2(const std::string_view& key, uint32_t seed);
        static uint32_t MurmurHash2(const char* key, uint32_t seed);

        static uint64_t MurmurHash2_64(const void * key, size_t len, uint64_t seed);
        static uint64_t MurmurHash2_64(const UTF8String & key, uint64_t seed);
        static uint64_t MurmurHash2_64(const std::string_view& key, uint64_t seed);
        static uint64_t MurmurHash2_64(const char* key, uint64_t seed);
    };
    template<typename T>
    concept Hashable = requires(T a, uint64_t seed)
    {
        { a.Hash(seed) } -> std::convertible_to<uint64_t>;
    };
    template<typename T>
    concept PrimitiveType = std::is_same_v<T, bool> || std::is_same_v<T, char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, short> || std::is_same_v<T, unsigned short> || std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_same_v<T, long> || std::is_same_v<T, unsigned long> || std::is_same_v<T, long long> || std::is_same_v<T, unsigned long long> || std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double> || std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t> || std::is_same_v<T, char8_t>;


    template<typename T>
    uint64_t Hash(const T& value, uint64_t seed = 0);

    template<Hashable T>
    uint64_t Hash(const T& value, uint64_t seed = 0)
    {
        return value.Hash(seed);
    }
    template<std::ranges::range T>
    requires (!std::convertible_to<T, String>)
    uint64_t Hash(const T& value, uint64_t seed = 0)
    {
        uint64_t hash = seed;
        for(const auto& v : value)
        {
            uint64_t h = Hash(v, hash);
            hash = HashAlgorithm::MurmurHash2_64(&h, sizeof(uint64_t), hash);
        }
        return hash;
    }

    template<PrimitiveType T>
    uint64_t Hash(const T& value, uint64_t seed = 0)
    {
        return HashAlgorithm::MurmurHash2_64(&value, sizeof(T), seed);
    }
    namespace Reflection
    {
        template<Reflection::HasReflection T>
        requires (!Hashable<T> && !PrimitiveType<T>)
        [[deprecated("Can not be used with Types, that use dynamic memory or other layers of indirection. Better write custom Hash function in class")]]
        uint64_t Hash(const T &value, uint64_t seed = 0)
        {
            uint64_t hash = seed;
            for (auto &member: Reflection::TypeResolver::Get<T>().Members)
            {
                void *memberPtr = (char *) &value + member.Offset;
                hash= HashAlgorithm::MurmurHash2_64(memberPtr, member.Type->Size, hash);
            }
            return hash;
        }
    }

} // BeeEngine

namespace std {
    template<BeeEngine::Hashable T>
    struct hash<T> {
        size_t operator()(const T& obj) const {
            return static_cast<size_t>(BeeEngine::Hash(obj));
        }
    };
} // std