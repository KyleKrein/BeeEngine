//
// Created by alexl on 01.08.2023.
//

#include "MUtils.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/Color4.h"
#include "Core/TypeDefines.h"
#include "MField.h"
#include "MTypes.h"
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace BeeEngine
{
    MType MUtils::ManagedNameToMType(const String& name)
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
            {"BeeEngine.Color", MType::Color},
            {"BeeEngine.Entity", MType::Entity},

            {"BeeEngine.Internal.AssetHandle", MType::AssetHandle},
            {"BeeEngine.Asset", MType::Asset},
            {"BeeEngine.Texture2D", MType::Texture2D},
            {"BeeEngine.Font", MType::Font},
            {"BeeEngine.Prefab", MType::Prefab},
        };
        if (MTypeMap.contains(name))
            return MTypeMap.at(name);
        if (name.ends_with('*'))
            return MType::Ptr;
        if (name.starts_with("System.Collections.Generic.Dictionary"))
            return MType::Dictionary;
        if (name.starts_with("System.Collections.Generic.List"))
            return MType::List;
        return MType::None;
    }

    MType MUtils::StringToMType(const String& name)
    {
        static std::unordered_map<std::string_view, MType> MTypeMap = {
            {"Void", MType::Void},
            {"Boolean", MType::Boolean},
            {"Char", MType::Char},
            {"SByte", MType::SByte},
            {"Byte", MType::Byte},
            {"Int16", MType::Int16},
            {"UInt16", MType::UInt16},
            {"Int32", MType::Int32},
            {"UInt32", MType::UInt32},
            {"Int64", MType::Int64},
            {"UInt64", MType::UInt64},
            {"Single", MType::Single},
            {"Double", MType::Double},
            {"String", MType::String},
            {"Object", MType::Object},
            {"Array", MType::Array},
            {"Dictionary", MType::Dictionary},
            {"List", MType::List},

            {"Vector2", MType::Vector2},
            {"Vector3", MType::Vector3},
            {"Vector4", MType::Vector4},
            {"Color", MType::Color},
            {"Entity", MType::Entity},

            {"AssetHandle", MType::AssetHandle},
            {"Asset", MType::Asset},
            {"Texture2D", MType::Texture2D},
            {"Font", MType::Font},
            {"Prefab", MType::Prefab},

            {"int", MType::Int32},
            {"float", MType::Single},
            {"double", MType::Double},
            {"bool", MType::Boolean},
            {"char", MType::Char},
            {"uint", MType::UInt32},
            {"long", MType::Int64},
            {"ulong", MType::UInt64},
            {"short", MType::Int16},
            {"ushort", MType::UInt16},
            {"byte", MType::Byte},
            {"sbyte", MType::SByte},
            {"string", MType::String},
            {"object", MType::Object},
        };
        if (MTypeMap.contains(name))
            return MTypeMap.at(name);
        if (name.ends_with('*'))
            return MType::Ptr;
        return MType::None;
    }

    /*MType MUtils::MonoTypeToMType(MonoType *monoType)
    {
        char* typeName = mono_type_get_name(monoType);
        MType type = ManagedNameToMType(typeName);
        if(type == MType::None)
        {
            BeeCoreTrace("Unknown type: {}", typeName);
        }
        mono_free(typeName);
        return type;
    }*/

    const char* MUtils::MTypeToString(MType type)
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
            case MType::Color:
                return "Color";
            case MType::Entity:
                return "Entity";
            case MType::AssetHandle:
                return "AssetHandle";
            case MType::Asset:
                return "Asset";
            case MType::Texture2D:
                return "Texture2D";
            case MType::Font:
                return "Font";
            case MType::Prefab:
                return "Prefab";

            case MType::None:
                break;
        }
        return "unknown";
    }

    bool MUtils::IsSutableForEdit(const MField& field)
    {
        return field.GetVisibility() == MVisibility_Public && !field.IsStatic() && field.GetType() != MType::None &&
               field.GetType() != MType::Ptr && field.GetType() != MType::Void;
    }

    size_t MUtils::SizeOfMType(MType type)
    {
        switch (type)
        {
            case MType::Char:
                return 2;
            case MType::Boolean:
            case MType::SByte:
            case MType::Byte:
                return 1;
            case MType::Int16:
                return sizeof(int16_t);
            case MType::UInt16:
                return sizeof(uint16_t);
            case MType::Int32:
                return sizeof(int32_t);
            case MType::UInt32:
                return sizeof(uint32_t);
            case MType::Int64:
                return sizeof(int64_t);
            case MType::UInt64:
                return sizeof(uint64_t);
            case MType::Single:
                return sizeof(float);
            case MType::Double:
                return sizeof(double);
            case MType::String:
                return sizeof(uint64_t);
            case MType::Ptr:
                return sizeof(void*);
            case MType::Dictionary:
                return sizeof(uint64_t);
            case MType::Array:
                return sizeof(uint64_t);
            case MType::List:
                return sizeof(uint64_t);
            case MType::Object:
                return sizeof(uint64_t);
            case MType::Vector2:
                return sizeof(glm::vec2);
            case MType::Vector3:
                return sizeof(glm::vec3);
            case MType::Vector4:
                return sizeof(glm::vec4);
            case MType::Color:
                return sizeof(Color4);
            case MType::Entity:
                return sizeof(uint64_t);
            case MType::Asset:
            case MType::Texture2D:
            case MType::Font:
            case MType::Prefab:
                return sizeof(AssetHandle);

            case MType::None:
            case MType::Void:
                return 0;
        }
        return 0;
    }

    bool MUtils::ShouldFreeGCHandle(const MField& field)
    {
        MType type = field.GetType();
        return type == MType::String || type == MType::Array || type == MType::Dictionary || type == MType::List ||
               type == MType::Object || type == MType::Asset || type == MType::Texture2D || type == MType::Font ||
               type == MType::Prefab;
    }

    bool MUtils::IsValueType(MType type)
    {
        return type == MType::Char || type == MType::Boolean || type == MType::SByte || type == MType::Byte ||
               type == MType::Int16 || type == MType::UInt16 || type == MType::Int32 || type == MType::UInt32 ||
               type == MType::Int64 || type == MType::UInt64 || type == MType::Single || type == MType::Double ||
               type == MType::Vector2 || type == MType::Vector3 || type == MType::Vector4 || type == MType::Color;
    }
} // namespace BeeEngine
