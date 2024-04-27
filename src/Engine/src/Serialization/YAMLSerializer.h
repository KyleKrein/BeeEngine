//
// Created by alexl on 17.10.2023.
//

#pragma once

#include "ISerializer.h"
#include "Serializable.h"
#include "Core/String.h"

namespace BeeEngine::Serialization
{
    class YAMLSerializer final
    {
    public:
        YAMLSerializer();
        ~YAMLSerializer();
        YAMLSerializer(const YAMLSerializer&) = delete;
        YAMLSerializer& operator=(const YAMLSerializer&) = delete;
        YAMLSerializer(YAMLSerializer&&) = default;
        YAMLSerializer& operator=(YAMLSerializer&&) = default;

        friend YAMLSerializer &operator&(YAMLSerializer &, Marker marker);

        friend YAMLSerializer &operator&(YAMLSerializer &, UTF8String& value);
        friend YAMLSerializer &operator&(YAMLSerializer &, int32_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, uint32_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, int64_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, uint64_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, float value);
        friend YAMLSerializer &operator&(YAMLSerializer &, double value);
        friend YAMLSerializer &operator&(YAMLSerializer &, bool value);
        friend YAMLSerializer &operator&(YAMLSerializer &, char value);
        friend YAMLSerializer &operator&(YAMLSerializer &, const char* value);
        friend YAMLSerializer &operator&(YAMLSerializer &, char16_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, char32_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, char8_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, int16_t value);
        friend YAMLSerializer &operator&(YAMLSerializer &, uint16_t value);

        template<class T>
        friend YAMLSerializer &operator&(YAMLSerializer &, Serialization::Value<T> value);
        friend YAMLSerializer &operator&(YAMLSerializer &, Serialization::Key key);

        //friend YAMLSerializer &operator&(YAMLSerializer &, std::ranges::range auto& value);

        friend YAMLSerializer &operator&(YAMLSerializer & serializer, EnumType auto value)
        {
            serializer.SerializeString(EnumToString(value));
            return serializer;
        }

        String ToString();

    private:
        void SerializeString(const String& string);
        void SerializeValue();
    private:
        struct YAMLSerializerData* m_Data;
    };

    template<class T>
    YAMLSerializer &operator&(YAMLSerializer & serializer, Serialization::Value<T> value)
    {
        serializer.SerializeValue();
        serializer & value.Val;
        return serializer;
    }

}
