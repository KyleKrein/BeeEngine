//
// Created by alexl on 01.08.2023.
//

#pragma once
#include "MTypes.h"

namespace BeeEngine
{
    class MClass;
    class MObject;
    class MField
    {
        friend class MClass;
        friend class MObject;
    public:
        MField(MClass& mClass, const String& name, MType type, MVisibility visibility, bool isStatic, uint64_t fieldID)
            : m_Class(&mClass), m_Name(name), m_Type(type), m_Visibility(visibility), m_IsStatic(isStatic) {}
            bool IsStatic() const { return m_IsStatic; }
            MType GetType() const { return m_Type; }
            MVisibility GetVisibility() const { return m_Visibility; }
            const String& GetName() const { return m_Name; }
            MClass& GetClass() const { return *m_Class; }
            //operator MonoClassField*() const { return m_MonoField; }
        uint64_t GetFieldID() const { return m_FieldID; }
    private:
        MClass* m_Class = nullptr;
        uint64_t m_FieldID = 0;
        MType m_Type;
        MVisibility m_Visibility;
        //MonoClassField* m_MonoField = nullptr;
        String m_Name;
        bool m_IsStatic;
    };
}