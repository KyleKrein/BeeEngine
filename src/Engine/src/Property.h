//
// Created by alexl on 05.06.2023.
//

#pragma once

#include <algorithm>

#define GETTER [this]() -> const auto& { return this->Value; }
#define SETTER [this](const auto& value) { this->Value = value; }

namespace BeeEngine
{
    template<typename T>
    class Property
    {
    public:
        Property(T& value, std::function<const T&()> getter, std::function<void(const T&)> setter)
        : m_IsReadOnly(false) ,m_Setter(setter),m_Getter(getter), Value(value) {}
        Property(T& value, std::function<const T&()> getter)
        :m_IsReadOnly(true) ,m_Getter(getter), Value(value) {}
        Property(T&& value, std::function<const T&()> getter, std::function<void(const T&)> setter)
        :m_IsReadOnly(false) ,m_Setter(setter), m_Getter(getter), Value(std::move(value)) {}
        Property(T&& value, std::function<const T&()> getter)
        :m_IsReadOnly(true) ,m_Getter(getter), Value(std::move(value)) {}
        Property(std::function<const T&()> getter, std::function<void(const T&)> setter)
        :m_IsReadOnly(false) , m_Setter(setter), m_Getter(getter), Value() {}
        Property(std::function<const T&()> getter)
        :m_IsReadOnly(false), m_Getter(getter), Value() {}

        Property(Property& other) = delete;
        Property(Property&& other) = delete;

        inline const T& Get() const
        {
            return m_Getter();
        }
        inline void Set(const T& value)
        {
            m_Setter(value);
        }
        inline void Set(const T&& value)
        {
            m_Setter(std::move(value));
        }
        inline Property& operator=(const T& value)
        {
            Set(value);
            return *this;
        }
        inline Property& operator=(T&& value)
        {
            Set(std::move(value));
            return *this;
        }
        inline operator const T&() const
        {
            return Get();
        }
        inline operator T() const
        {
            return Get();
        }

        T& value() { return Value; }
    private:
        bool m_IsReadOnly;
        std::function<void(const T&)> m_Setter = [this](const T& value){ static_assert(!m_IsReadOnly);};
        std::function<const T&()> m_Getter;

        T Value;
    };
}