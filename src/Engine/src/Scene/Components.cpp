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
    REFLECT_STRUCT_BEGIN(UUIDComponent)
                        REFLECT_STRUCT_MEMBER(ID)
    REFLECT_STRUCT_END()
    REFLECT_STRUCT_BEGIN(TagComponent)
                        REFLECT_STRUCT_MEMBER(Tag)
    REFLECT_STRUCT_END()
    REFLECT_STRUCT_BEGIN(TransformComponent)
                        REFLECT_STRUCT_MEMBER(Translation)
                        REFLECT_STRUCT_MEMBER(Rotation)
                        REFLECT_STRUCT_MEMBER(Scale)
    //REFLECT_MEMBERS_END()
    //REFLECT_METHODS_BEGIN()
    //                    REFLECT_METHOD(GetTransform)
    //                    REFLECT_METHOD(SetTransform)
    REFLECT_STRUCT_END()

    REFLECT_STRUCT_BEGIN(CameraComponent)
                        REFLECT_STRUCT_MEMBER(Camera)
                        REFLECT_STRUCT_MEMBER(Primary)
                        REFLECT_STRUCT_MEMBER(FixedAspectRatio)
    REFLECT_STRUCT_END()
}