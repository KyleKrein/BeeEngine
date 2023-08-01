//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "MObject.h"
#include "MMethod.h"

namespace BeeEngine
{
    class GameScript
    {
    public:
        GameScript(MClass& mClass, class Entity entity);
        void InvokeOnCreate();
        void InvokeOnDestroy();
        void InvokeOnUpdate();
        MObject& GetMObject() { return m_Instance; }
        std::unordered_map<std::string_view, class MField*>& GetEditableFields() { return m_EditableFields; }

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
        std::unordered_map<std::string_view , class MField*> m_EditableFields;

        void SelectEditableFields(MClass &aClass);
    };
}