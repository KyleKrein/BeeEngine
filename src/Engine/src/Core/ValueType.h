//
// Created by alexl on 16.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
namespace BeeEngine
{
    template<typename Type, Type min, Type max>
    requires (std::is_arithmetic_v<Type> || std::is_enum_v<Type>) && (min < max)
    class ValueType
    {
    public:
        ValueType(Type value)
        {
            static_assert(value >= min && value <= max, "Value is out of range");
            ApplyValue(value);
        }
        ValueType& operator=(Type value)
        {
            static_assert(value >= min && value <= max, "Value is out of range");
            ApplyValue(value);
            return *this;
        }
        operator Type() const
        {
            return m_Value;
        }
    private:
        Type m_Value;

        void ApplyValue(Type value)
        {
            BeeExpects(value >= min && value <= max);
            m_Value = value;
        }
    };
}