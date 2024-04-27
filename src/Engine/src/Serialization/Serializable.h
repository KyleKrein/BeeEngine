//
// Created by alexl on 17.10.2023.
//

#pragma once
#include <type_traits>

namespace BeeEngine::Serialization
{


    template<typename T, typename Archive>
    concept Serializable = requires(T a, Archive& serializer)
    {
        { a.Serialize(serializer) };
    };

    template<typename Archive,Serializable<Archive> T>
    Archive& operator & (Archive& serializer, T& value)
    {
        value.Serialize(serializer);
        return serializer;
    }

    template<typename T>
    concept EnumType = std::is_enum_v<T>;
}