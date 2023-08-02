//
// Created by alexl on 02.08.2023.
//
#include "Components.h"
#include "Scripting/MClass.h"
#include "Scripting/ScriptingEngine.h"
#include "Scripting/GameScript.h"
namespace BeeEngine
{

    void ScriptComponent::SetClass(MClass *mClass) noexcept
    {
        if(mClass == Class)
            return;
        if(mClass == nullptr)
        {
            Class = nullptr;
            EditableFields.clear();
            return;
        }
        Class = mClass;
        EditableFields = ScriptingEngine::GetDefaultScriptFields(Class);
    }
}