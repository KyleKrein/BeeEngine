//
// Created by Aleksandr on 23.12.2023.
//

#include "MAssembly.h"
#include "MClass.h"
#include "FileSystem/File.h"
#include "Core/Logging/Log.h"
#include "ScriptingEngine.h"
#include "MObject.h"
#include "MMethod.h"
#include "GameScript.h"
#include "Scene/Entity.h"
#include "MField.h"
#include "MUtils.h"
#include "NativeToManaged.h"
#include "Scene/Components.h"

namespace BeeEngine
{
    MObject MClass::Instantiate()
    {
        return {*this};
    }

    MMethod& MClass::GetMethod(const String& name, int paramCount)
    {
        if(!m_Methods.contains(name))
            m_Methods.emplace(name, MMethod(*this, name, paramCount));
        return m_Methods.at(name);
    }

    MField& MClass::GetField(const String& name)
    {
        return m_Fields.at(name);
    }

    bool MClass::IsDerivedFrom(MClass& other) const
    {
        return false;
    }

    MClass::MClass(const String& name, const String& ns, uint64_t classId)
    : m_Name(name), m_Namespace(ns), m_ClassID(classId)
    {
        m_FullName = m_Namespace + "." + m_Name;
    }

    GameScript::GameScript(MClass& mClass, Entity entity, const String& locale)
        : m_Instance(mClass.Instantiate())
    {
    }

    void GameScript::InvokeOnCreate()
    {
    }

    void GameScript::InvokeOnDestroy()
    {
    }

    void GameScript::InvokeOnUpdate()
    {
    }

    void GameScript::CopyFieldsData(std::vector<GameScriptField>& aClass, const String& locale)
    {
    }

    MAssembly::MAssembly()
    {
    }

    MAssembly::MAssembly(uint64_t contextId, const Path& path, bool debug)
    : m_Path(path), m_ContextID(contextId)
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

    MAssembly::~MAssembly()
    {
    }

    MAssembly::MAssembly(const MAssembly&)
    {
    }

    MAssembly& MAssembly::operator=(const MAssembly& other)
    {
        m_Classes = other.m_Classes;
        m_Path = other.m_Path;
        return *this;
    }

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
        for(auto classId : classIds)
        {
            String name = NativeToManaged::GetClassName(m_ContextID, m_AssemblyID, classId);
            static const char* anType = "<>f__AnonymousType";
            //static const char* displayType = "<>c__DisplayClass"; //Anonymous type for lambda
            static const char* compilerGenerated = "<>c";
            static const char* enumerator = "<GetEnumerator>";
            static const char* keyHolder = "KeyHolder";
            if(name.contains("<Module>") || name.contains(anType)
            || name.contains(compilerGenerated) || name.contains(enumerator)
            || name.contains(keyHolder))
                continue;
            String ns = NativeToManaged::GetClassNamespace(m_ContextID, m_AssemblyID, classId);
            if(ns.empty())
                continue;
            m_Classes.emplace_back(CreateRef<MClass>(std::move(name), std::move(ns), classId));
        }
    }

    MMethod::MMethod(MClass& mClass, const String& name, int paramCount)
    {
    }

    MMethod::~MMethod()
    {
    }

    MObject::MObject(MClass& object)
    {
    }

    MObject::~MObject()
    {
    }

    MClass& MObject::GetClass()
    {
        return *m_Class;
    }

    void MObject::Invoke(MMethod& method, void** params)
    {
    }

    void MObject::SetFieldValue(MField& field, void* value)
    {
    }

    bool MObject::GetFieldValue(MField& field, void* value)
    {
        throw std::runtime_error("Not implemented");
    }

    String MObject::GetFieldStringValue(MField& field)
    {
        throw std::runtime_error("Not implemented");
    }
}
