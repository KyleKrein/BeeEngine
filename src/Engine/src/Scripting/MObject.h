//
// Created by alexl on 30.07.2023.
//

#pragma once

namespace BeeEngine
{
    class MClass;
    class MField;

    class MFieldValue
    {
    public:
        MFieldValue(bool shouldBeFreed, void* value, MType type)
        : m_ShouldBeFreed(shouldBeFreed), m_Value(value)
        {}
        ~MFieldValue();
        void* GetValue() const
        {
            return m_Value;
        }
        MType GetType() const
        {
            return m_Type;
        }
        operator void*() const
        {
            return m_Value;
        }
    private:
        void* m_Value = nullptr;
        MType m_Type = MType::None;
        bool m_ShouldBeFreed = false;
    };
    class MObject
    {
    public:
        MObject(const MClass& object);
        //MObject(MonoObject* object);
        ~MObject();
        MClass& GetClass();
        void* Invoke(class MMethod& method, void** params);

        void SetFieldValue(MField& field, void* value);
        MFieldValue GetFieldValue(MField& field);
        void* GetFieldValueUnsafe(MField& field);
        String GetFieldStringValue(MField& field);

        void* GetHandle()
        {
            return m_Handle;
        }

    private:
        //MonoObject* m_MonoObject = nullptr;
        void* m_Handle = nullptr;
        MClass* m_Class = nullptr;
    };
}
