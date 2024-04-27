//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "MObject.h"
#include "MMethod.h"

namespace BeeEngine
{
    class GameScriptField
    {
    public:
        constexpr static size_t MAX_FIELD_SIZE = 16;
        GameScriptField(MField& field): m_Field(&field) {}
        MField& GetMField() { return *m_Field; }

        template<class T>
        void SetData(const T& data)
        {
            static_assert(sizeof (T)<= MAX_FIELD_SIZE, "Type is too large");
            memcpy(m_Buffer, &data, sizeof (T));
        }
        template<>
        void SetData<MFieldValue>(const MFieldValue& data)
        {
            memcpy(m_Buffer, data.GetValuePtr(), MUtils::SizeOfMType(data.GetType()));
        }

        void SetData(void* data)
        {
            memcpy(m_Buffer, data, MAX_FIELD_SIZE);
        }

        template<class T>
        [[nodiscard]] const T& GetData()
        {
            static_assert(sizeof(T) <= MAX_FIELD_SIZE, "Type is too large");
            return *(T*)m_Buffer;
        }

        [[nodiscard]] void* GetData()
        {
            return m_Buffer;
        }

    private:
        MField* m_Field;
        byte m_Buffer[MAX_FIELD_SIZE];
    };
    class GameScript
    {
    public:
        //using OnFunction = void(*)();

        GameScript(MClass& mClass, class Entity entity, const String& locale);
        void InvokeOnCreate();
        void InvokeOnDestroy();
        void InvokeOnUpdate();
        MObject& GetMObject() { return m_Instance; }

        ~GameScript()
        {
            if(m_OnDestroy)
                InvokeOnDestroy();
        }
    private:
        MObject m_Instance;

        MMethod* m_OnCreate = nullptr;
        MMethod* m_OnDestroy = nullptr;
        MMethod* m_OnUpdate = nullptr;

        void CopyFieldsData(std::vector<GameScriptField> &aClass, const String& locale);
    };
}
