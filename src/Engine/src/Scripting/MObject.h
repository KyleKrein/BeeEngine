//
// Created by alexl on 30.07.2023.
//

#pragma once

extern "C"
{
    typedef struct _MonoObject MonoObject;
}

namespace BeeEngine
{
    class MClass;

    class MObject
    {
    public:
        MObject(MClass& object);
        ~MObject();
        MClass& GetClass();
        void Invoke(class MMethod& method, void** params);

        void SetFieldValue(class MField& field, void* value);

        bool GetFieldValue(class MField& field, void* value);
        String GetFieldStringValue(class MField& field);
    private:
        MonoObject* m_MonoObject = nullptr;
        MClass* m_Class = nullptr;
    };
}