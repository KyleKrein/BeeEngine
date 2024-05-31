//
// Created by alexl on 17.10.2023.
//

#pragma once
#include "Serializable.h"

namespace BeeEngine::Serialization
{
    struct Key
    {
        Key(const char* key) : String(key) {}
        const char* String;
    };

    template <typename T>
    struct Value
    {
        Value(const T& val) : Val(val) {}
        const T& Val;
    };
    enum class Marker
    {
        BeginMap,
        EndMap,
        BeginSeq,
        EndSeq
    };
} // namespace BeeEngine::Serialization
