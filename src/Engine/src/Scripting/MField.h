//
// Created by alexl on 01.08.2023.
//

#pragma once
#include "MTypes.h"
extern "C"
{
    typedef struct _MonoField MonoField;
    typedef struct _MonoClassField MonoClassField;
}

namespace BeeEngine
{
    class MClass;
    class MObject;
    class MField
    {
        friend class MClass;
        friend class MObject;
    public:
        MField(MClass& mClass, const String& name, MType type, MVisibility visibility, MonoClassField* monoField, bool isStatic)
            : m_Class(&mClass), m_Name(name), m_Type(type), m_Visibility(visibility), m_MonoField(monoField), m_IsStatic(isStatic) {}
            bool IsStatic() const { return m_IsStatic; }
            MType GetType() const { return m_Type; }
            MVisibility GetVisibility() const { return m_Visibility; }
            const String& GetName() const { return m_Name; }
            MClass& GetClass() const { return *m_Class; }
            operator MonoClassField*() const { return m_MonoField; }
    private:
        MClass* m_Class = nullptr;
        MType m_Type;
        MVisibility m_Visibility;
        MonoClassField* m_MonoField = nullptr;
        String m_Name;
        bool m_IsStatic;
    };
}