//
// Created by alexl on 17.10.2023.
//

#include "YAMLSerializer.h"
#include "Core/String.h"
#include "Core/ToString.h"
#include "yaml-cpp/yaml.h"

namespace BeeEngine::Serialization
{
    struct YAMLSerializerData
    {
        YAML::Emitter Out;
    };
    YAMLSerializer::YAMLSerializer() : m_Data(new YAMLSerializerData()) {}

    YAMLSerializer::~YAMLSerializer()
    {
        delete m_Data;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, Marker marker)
    {
        switch (marker)
        {
            case Marker::BeginMap:
                serializer.m_Data->Out << YAML::BeginMap;
                break;
            case Marker::EndMap:
                serializer.m_Data->Out << YAML::EndMap;
                break;
            case Marker::BeginSeq:
                serializer.m_Data->Out << YAML::BeginSeq;
                break;
            case Marker::EndSeq:
                serializer.m_Data->Out << YAML::EndSeq;
                break;
        }
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, UTF8String& value)
    {
        serializer.m_Data->Out << value.c_str();
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, int32_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, uint32_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, int64_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, uint64_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, float value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, double value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, bool value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, char value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, const char* value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, char16_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, char32_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, char8_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, int16_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, uint16_t value)
    {
        serializer.m_Data->Out << value;
        return serializer;
    }

    /*YAMLSerializer &operator&(YAMLSerializer & serializer, std::ranges::range auto &value)
    {
        serializer.m_Data->Out << YAML::Flow;
        serializer.m_Data->Out << YAML::BeginSeq;
        for (const auto &v : value)
        {
            serializer & Marker::Value;
            serializer & v;
        }
        serializer.m_Data->Out << YAML::EndSeq;
        return serializer;
    }*/

    String YAMLSerializer::ToString()
    {
        return m_Data->Out.c_str();
    }

    void YAMLSerializer::SerializeString(const String& string)
    {
        m_Data->Out << string.c_str();
    }

    YAMLSerializer& operator&(YAMLSerializer& serializer, Serialization::Key key)
    {
        serializer.m_Data->Out << YAML::Key << key.String;
        return serializer;
    }

    void YAMLSerializer::SerializeValue()
    {
        m_Data->Out << YAML::Value;
    }
} // namespace BeeEngine::Serialization
