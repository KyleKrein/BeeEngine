//
// Created by alexl on 01.08.2023.
//

#include "MUtils.h"
#include "MTypes.h"
#include "Core/TypeDefines.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "MField.h"

namespace BeeEngine
{
    MType MUtils::StringToMType(const String& name)
    {
        static std::unordered_map<String, MType> MTypeMap = {
                {"System.Void", MType::Void},
                {"System.Boolean", MType::Boolean},
                {"System.Char", MType::Char},
                {"System.SByte", MType::SByte},
                {"System.Byte", MType::Byte},
                {"System.Int16", MType::Int16},
                {"System.UInt16", MType::UInt16},
                {"System.Int32", MType::Int32},
                {"System.UInt32", MType::UInt32},
                {"System.Int64", MType::Int64},
                {"System.UInt64", MType::UInt64},
                {"System.Single", MType::Single},
                {"System.Double", MType::Double},
                {"System.String", MType::String},
                {"System.Object", MType::Object},
                {"System.Array", MType::Array},

                {"BeeEngine.Math.Vector2", MType::Vector2},
                {"BeeEngine.Math.Vector3", MType::Vector3},
                {"BeeEngine.Math.Vector4", MType::Vector4},
                {"BeeEngine.Entity", MType::Entity},
        };
        if(MTypeMap.contains(name))
            return MTypeMap.at(name);
        if(name.ends_with('*'))
            return MType::Ptr;
        if(name.starts_with("System.Collections.Generic.Dictionary"))
            return MType::Dictionary;
        if(name.starts_with("System.Collections.Generic.List"))
            return MType::List;
        return MType::None;
    }

    MType MUtils::MonoTypeToMType(MonoType *monoType)
    {
        char* typeName = mono_type_get_name(monoType);
        MType type = StringToMType(typeName);
        mono_free(typeName);
        if(type == MType::None)
        {
            BeeCoreError("Unknown type: {}", typeName);
        }
        return type;
    }

    const char *MUtils::MTypeToString(MType type)
    {
        switch (type)
        {
            case MType::Void:
                return "void";
            case MType::Boolean:
                return "bool";
            case MType::Char:
                return "char";
            case MType::SByte:
                return "sbyte";
            case MType::Byte:
                return "byte";
            case MType::Int16:
                return "short";
            case MType::UInt16:
                return "ushort";
            case MType::Int32:
                return "int";
            case MType::UInt32:
                return "uint";
            case MType::Int64:
                return "long";
            case MType::UInt64:
                return "ulong";
            case MType::Single:
                return "float";
            case MType::Double:
                return "double";
            case MType::String:
                return "string";
            case MType::Ptr:
                return "void*";
            case MType::Object:
                return "object";
            case MType::Array:
                return "Array";
            case MType::Dictionary:
                return "Dictionary";
            case MType::List:
                return "List";

            case MType::Vector2:
                return "Vector2";
            case MType::Vector3:
                return "Vector3";
            case MType::Vector4:
                return "Vector4";
            case MType::Entity:
                return "Entity";

            case MType::None:
                break;
        }
        return "unknown";
    }

    MVisibility MUtils::MonoFieldFlagsToVisibility(uint32_t flags)
    {
        if(flags & FIELD_ATTRIBUTE_PRIVATE)
            return MVisibility::Private;
        if(flags & FIELD_ATTRIBUTE_PUBLIC)
            return MVisibility::Public;
        if(flags & FIELD_ATTRIBUTE_FAMILY)
            return MVisibility::Protected;
        if(flags & FIELD_ATTRIBUTE_ASSEMBLY)
            return MVisibility::Internal;
        if(flags & FIELD_ATTRIBUTE_FAM_AND_ASSEM)
            return MVisibility::ProtectedInternal;
        if(flags & FIELD_ATTRIBUTE_FAM_OR_ASSEM)
            return MVisibility::ProtectedInternal;
        return MVisibility::Private;
    }

    bool MUtils::IsSutableForEdit(const MField& field)
    {
        return field.GetVisibility() == MVisibility::Public &&
               !field.IsStatic() &&
               field.GetType() != MType::None &&
               field.GetType() != MType::Ptr &&
               field.GetType() != MType::Void;
    }

    MVisibility MUtils::MonoMethodFlagsToVisibility(uint32_t flags)
    {
        if(flags & METHOD_ATTRIBUTE_PRIVATE)
            return MVisibility::Private;
        if(flags & METHOD_ATTRIBUTE_PUBLIC)
            return MVisibility::Public;
        if(flags & METHOD_ATTRIBUTE_FAMILY)
            return MVisibility::Protected;
        if(flags & METHOD_ATTRIBUTE_ASSEM)
            return MVisibility::Internal;
        if(flags & METHOD_ATTRIBUTE_FAM_AND_ASSEM)
            return MVisibility::ProtectedInternal;
        if(flags & METHOD_ATTRIBUTE_FAM_OR_ASSEM)
            return MVisibility::ProtectedInternal;
    }
}
