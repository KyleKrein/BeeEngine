//
// Created by alexl on 30.07.2023.
//

#pragma once

namespace BeeEngine
{
    class MClass;
    class MField;

    class MObject
    {
    public:
        MObject(MClass& object);
        //MObject(MonoObject* object);
        ~MObject();
        MClass& GetClass();
        void Invoke(class MMethod& method, void** params);

        void SetFieldValue(MField& field, void* value);
        bool GetFieldValue(MField& field, void* value);
        String GetFieldStringValue(MField& field);
    private:
        //MonoObject* m_MonoObject = nullptr;
        uint32_t m_Handle = 0;
        MClass* m_Class = nullptr;
    };
}
