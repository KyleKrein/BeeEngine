//
// Created by Aleksandr on 23.12.2023.
//

#include "Core/Logging/Log.h"
#include "FileSystem/File.h"
#include "GameScript.h"
#include "MAssembly.h"
#include "MClass.h"
#include "MField.h"
#include "MMethod.h"
#include "MObject.h"
#include "MUtils.h"
#include "NativeToManaged.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "ScriptingEngine.h"

namespace BeeEngine
{
    MObject MClass::Instantiate()
    {
        return {*this};
    }

    MMethod& MClass::GetMethod(const String& name, ManagedBindingFlags flags)
    {
        if (!m_Methods.contains(name))
            m_Methods.emplace(name, MMethod(*this, name, flags));
        return m_Methods.at(name);
    }

    MField& MClass::GetField(const String& name)
    {
        return m_Fields.at(name);
    }

    bool MClass::IsDerivedFrom(const MClass& other) const
    {
        return NativeToManaged::ClassIsDerivedFrom(
            m_ContextID, m_AssemblyID, m_ClassID, other.m_ContextID, other.m_AssemblyID, other.m_ClassID);
    }

    MClass::MClass(const String& name, const String& ns, uint64_t contextId, uint64_t assemblyId, uint64_t classId)
        : m_Name(name), m_Namespace(ns), m_ContextID(contextId), m_AssemblyID(assemblyId), m_ClassID(classId)
    {
        m_FullName = m_Namespace + "." + m_Name;
        m_IsValueType = NativeToManaged::ClassIsValueType(m_ContextID, m_AssemblyID, m_ClassID);
        m_IsEnum = NativeToManaged::ClassIsEnum(m_ContextID, m_AssemblyID, m_ClassID);
        auto fields = NativeToManaged::ClassGetFields(
            m_ContextID,
            m_AssemblyID,
            m_ClassID,
            static_cast<ManagedBindingFlags>(ManagedBindingFlags_Public | ManagedBindingFlags_NonPublic |
                                             ManagedBindingFlags_Instance | ManagedBindingFlags_Static));
        for (auto field : fields)
        {
            String fieldName = NativeToManaged::FieldGetName(m_ContextID, m_AssemblyID, m_ClassID, field);
            MType fieldType =
                MUtils::StringToMType(NativeToManaged::FieldGetTypeName(m_ContextID, m_AssemblyID, m_ClassID, field));
            MFieldFlags fieldFlags = NativeToManaged::FieldGetFlags(m_ContextID, m_AssemblyID, m_ClassID, field);
            bool isStatic = (fieldFlags & MFieldFlags_Static) == MFieldFlags_Static;
            MVisibility visibility = ExtractMVisibility(fieldFlags);
            m_Fields.emplace(fieldName, MField(*this, fieldName, fieldType, visibility, isStatic, field));
        }
    }

    GameScript::GameScript(MClass& mClass, Entity entity, const String& locale) : m_Instance(mClass.Instantiate())
    {
        constexpr static ManagedBindingFlags flags = static_cast<ManagedBindingFlags>(
            ManagedBindingFlags_Public | ManagedBindingFlags_NonPublic | ManagedBindingFlags_Instance);
        auto& onCreate = mClass.GetMethod("OnCreate", flags);
        if (onCreate.IsValid())
            m_OnCreate = &onCreate;
        auto& onDestroy = mClass.GetMethod("OnDestroy", flags);
        if (onDestroy.IsValid())
            m_OnDestroy = &onDestroy;
        auto& onUpdate = mClass.GetMethod("OnUpdate", flags);
        if (onUpdate.IsValid())
            m_OnUpdate = &onUpdate;

        if (entity.HasComponent<ScriptComponent>())
        {
            auto& sc = entity.GetComponent<ScriptComponent>();
            CopyFieldsData(sc.EditableFields, locale);
        }
    }

    void GameScript::InvokeOnCreate()
    {
        if (!m_OnCreate)
            return;
        m_Instance.Invoke(*m_OnCreate, nullptr);
        m_OnCreate = nullptr; // to prevent double call
    }

    void GameScript::InvokeOnDestroy()
    {
        if (m_OnCreate)
            m_Instance.Invoke(*m_OnDestroy, nullptr);
    }

    void GameScript::InvokeOnUpdate()
    {
        if (m_OnUpdate)
            m_Instance.Invoke(*m_OnUpdate, nullptr);
    }

    static MType AssetTypeToMType(AssetType type)
    {
        switch (type)
        {
            case AssetType::Texture2D:
                return MType::Texture2D;
            case AssetType::Font:
                return MType::Font;
            default:
                return MType::Asset;
        }
    }

    void GameScript::CopyFieldsData(std::vector<GameScriptField>& aClass, const String& locale)
    {
        for (auto& field : aClass)
        {
            auto& mField = field.GetMField();
            auto type = mField.GetType();
            if (type == MType::Asset || type == MType::Texture2D || type == MType::Font || type == MType::Prefab)
            {
                AssetHandle handle = field.GetData<AssetHandle>();
                if (AssetManager::IsAssetHandleValid(handle))
                {
                    if (type == MType::Asset)
                        type = AssetTypeToMType(AssetManager::GetAsset<Asset>(handle, locale).GetType());
                    ScriptingEngine::SetAssetHandle(m_Instance, mField, handle, type);
                }
                continue;
            }
            m_Instance.SetFieldValue(mField, field.GetData());
        }
    }

    MAssembly::MAssembly() {}

    MAssembly::MAssembly(uint64_t contextId, const Path& path, bool debug) : m_Path(path), m_ContextID(contextId)
    {
        LoadAssembly();
        /*if(debug)
        {
            Path pdbPath = path;
            pdbPath.ReplaceExtension(".pdb");

            if (File::Exists(pdbPath))
            {
                auto pdbFile = File::ReadBinaryFile(pdbPath);
                mono_debug_open_image_from_memory(m_MonoImage, (const mono_byte*)pdbFile.data(), pdbFile.size());
                BeeCoreInfo("Loaded PDB {}", pdbPath.AsUTF8());
            }
        }*/
        GetClassesFromAssembly();
    }

    MAssembly::~MAssembly() {}

    MAssembly::MAssembly(MAssembly&& other) noexcept
    {
        m_Classes = std::move(other.m_Classes);
        m_Path = std::move(other.m_Path);
    }

    MAssembly& MAssembly::operator=(MAssembly&& other) noexcept
    {
        m_Classes = std::move(other.m_Classes);
        m_Path = std::move(other.m_Path);
        return *this;
    }

    std::vector<Ref<MClass>>& MAssembly::GetClasses()
    {
        return m_Classes;
    }

    void MAssembly::LoadAssembly()
    {
        m_AssemblyID = NativeToManaged::LoadAssemblyFromPath(m_ContextID, m_Path);
        BeeCoreAssert(m_AssemblyID, "Failed to load assembly {}", m_Path.AsUTF8());
    }

    void MAssembly::GetClassesFromAssembly()
    {
        std::vector<ManagedClassID> classIds = NativeToManaged::GetClassesFromAssembly(m_ContextID, m_AssemblyID);
        for (auto classId : classIds)
        {
            String name = NativeToManaged::GetClassName(m_ContextID, m_AssemblyID, classId);
            static const char* anType = "<>f__AnonymousType";
            // static const char* displayType = "<>c__DisplayClass"; //Anonymous type for lambda
            static const char* compilerGenerated = "<>c";
            static const char* enumerator = "<GetEnumerator>";
            static const char* keyHolder = "KeyHolder";
            if (name.contains("<Module>") || name.contains(anType) || name.contains(compilerGenerated) ||
                name.contains(enumerator) || name.contains(keyHolder))
                continue;
            String ns = NativeToManaged::GetClassNamespace(m_ContextID, m_AssemblyID, classId);
            if (ns.empty())
                continue;
            m_Classes.emplace_back(
                CreateRef<MClass>(std::move(name), std::move(ns), m_ContextID, m_AssemblyID, classId));
        }
    }

    MMethod::MMethod(MClass& mClass, const String& name, ManagedBindingFlags flags) : m_Class(&mClass), m_Name(name)
    {
        m_MethodID = NativeToManaged::MethodGetByName(
            m_Class->m_ContextID, m_Class->m_AssemblyID, m_Class->m_ClassID, m_Name, flags);
    }

    MMethod::~MMethod() {}

    void* MMethod::InvokeStatic(void** params)
    {
        // TODO: check if method is static
        return NativeToManaged::MethodInvoke(
            m_Class->m_ContextID, m_Class->m_AssemblyID, m_Class->m_ClassID, m_MethodID, nullptr, params);
    }

    MClass& MMethod::GetClass()
    {
        return *m_Class;
    }

    MFieldValue::~MFieldValue()
    {
        if (MUtils::IsValueType(m_Type))
            NativeToManaged::FreeIntPtr(m_Value);
        else
            NativeToManaged::ObjectFreeGCHandle(m_Value);
    }

    MObject::MObject(const MClass& object)
    {
        m_Handle = NativeToManaged::ObjectNewGCHandle(
            object.m_ContextID, object.m_AssemblyID, object.m_ClassID, GCHandleType::Normal);
    }

    MObject::~MObject()
    {
        NativeToManaged::ObjectFreeGCHandle(m_Handle);
    }

    MClass& MObject::GetClass()
    {
        return *m_Class;
    }

    void* MObject::Invoke(MMethod& method, void** params)
    {
        auto& mclass = method.GetClass();
        return NativeToManaged::MethodInvoke(
            mclass.m_ContextID, mclass.m_AssemblyID, mclass.m_ClassID, method.m_MethodID, m_Handle, params);
    }

    void MObject::SetFieldValue(MField& field, void* value)
    {
        auto& mclass = field.GetClass();
        NativeToManaged::FieldSetData(
            mclass.m_ContextID, mclass.m_AssemblyID, mclass.m_ClassID, field.m_FieldID, m_Handle, value);
    }

    MFieldValue MObject::GetFieldValue(MField& field)
    {
        return {GetFieldValueUnsafe(field), field.GetType()};
    }

    void* MObject::GetFieldValueUnsafe(MField& field)
    {
        auto& mclass = field.GetClass();
        return NativeToManaged::FieldGetData(
            mclass.m_ContextID, mclass.m_AssemblyID, mclass.m_ClassID, field.m_FieldID, m_Handle);
    }

    String MObject::GetFieldStringValue(MField& field)
    {
        throw std::runtime_error("Not implemented");
    }
} // namespace BeeEngine
