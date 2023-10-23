//
// Created by alexl on 30.07.2023.
//
#include "MAssembly.h"
#include "MClass.h"
#include "FileSystem/File.h"
#include "mono/metadata/assembly.h"
#include "Core/Logging/Log.h"
#include "mono/metadata/blob.h"
#include "mono/metadata/row-indexes.h"
#include "mono/metadata/metadata.h"
#include "ScriptingEngine.h"
#include "mono/metadata/loader.h"
#include "MObject.h"
#include "mono/metadata/appdomain.h"
#include "MMethod.h"
#include "GameScript.h"
#include <mono/metadata/object.h>
#include "Scene/Entity.h"
#include "MField.h"
#include "MUtils.h"
#include "mono/metadata/tabledefs.h"
#include "Scene/Components.h"
#include "mono/metadata/threads.h"
#include "mono/metadata/mono-debug.h"


namespace BeeEngine
{
    MAssembly::MAssembly(const Path& path, bool debug)
    : m_Path(path)
    {
        LoadAssembly();
        if(debug)
        {
            Path pdbPath = path;
            pdbPath.ReplaceExtension(".pdb");

            if (File::Exists(pdbPath))
            {
                auto pdbFile = File::ReadBinaryFile(pdbPath);
                mono_debug_open_image_from_memory(m_MonoImage, (const mono_byte*)pdbFile.data(), pdbFile.size());
                BeeCoreInfo("Loaded PDB {}", pdbPath.AsUTF8());
            }
        }
        mono_image_close(m_MonoImage);
        m_MonoImage = mono_assembly_get_image(m_MonoAssembly);
        GetClassesFromAssembly();
    }
    MAssembly::~MAssembly()
    {
        //UnloadAssembly();
    }
    void MAssembly::LoadAssembly()
    {
        auto fileData = File::ReadBinaryFile(m_Path.AsUTF8());

        MonoImageOpenStatus status;
        m_MonoImage = mono_image_open_from_data_full(reinterpret_cast<char *>(fileData.data()), fileData.size(), true,
                                                     &status, false);
        BeeCoreAssert(status == MONO_IMAGE_OK, "Failed to load assembly! Status: {}", mono_image_strerror(status));
        m_MonoAssembly = mono_assembly_load_from_full(m_MonoImage, m_Path.AsCString(), &status, false);
    }

    std::vector<Ref<MClass>>& MAssembly::GetClasses()
    {
        return m_Classes;
    }

    void MAssembly::GetClassesFromAssembly()
    {
        const MonoTableInfo *tableInfo = mono_image_get_table_info(m_MonoImage, MONO_TABLE_TYPEDEF);
        uint32_t rows = mono_table_info_get_rows(tableInfo);
        m_Classes.reserve(rows);
        for (uint32_t i = 0; i < rows; ++i)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(tableInfo, i, cols, MONO_TYPEDEF_SIZE);

            std::string name = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAME]);
            static const char* anType = "<>f__AnonymousType";
            //static const char* displayType = "<>c__DisplayClass"; //Anonymous type for lambda
            static const char* compilerGenerated = "<>c";
            static const char* enumerator = "<GetEnumerator>";
            static const char* keyHolder = "KeyHolder";
            if(name.contains("<Module>") || name.contains(anType)
            || name.contains(compilerGenerated) || name.contains(enumerator)
            || name.contains(keyHolder))
                continue;
            std::string ns = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAMESPACE]);
            if(ns.empty())
                continue;
            m_Classes.emplace_back(CreateRef<MClass>(name, ns, m_MonoImage));
        }
    }

    void MAssembly::UnloadAssembly()
    {
        if(m_MonoImage)
        {
            mono_image_close(m_MonoImage);
            m_MonoImage = nullptr;
        }
        if(m_MonoAssembly)
        {
            mono_assembly_close(m_MonoAssembly);
            m_MonoAssembly = nullptr;
        }
    }

    MAssembly::MAssembly(MAssembly &&other) noexcept
    {
        this->m_MonoImage = other.m_MonoImage;
        this->m_MonoAssembly = other.m_MonoAssembly;
        this->m_Path = std::move(other.m_Path);
        this->m_Classes = std::move(other.m_Classes);
        other.m_MonoImage = nullptr;
        other.m_MonoAssembly = nullptr;
    }

    MAssembly &MAssembly::operator=(MAssembly &&other) noexcept
    {
        this->m_MonoImage = other.m_MonoImage;
        this->m_MonoAssembly = other.m_MonoAssembly;
        this->m_Path = std::move(other.m_Path);
        this->m_Classes = std::move(other.m_Classes);
        other.m_MonoImage = nullptr;
        other.m_MonoAssembly = nullptr;
        return *this;
    }

    void ScriptingEngine::RegisterInternalCall(const std::string &name, void *method)
    {
        mono_add_internal_call(name.c_str(), method);
    }

    MObject::MObject(MClass &object)
    {
        m_Class = &object;
        MonoObject* instance = mono_object_new(mono_domain_get(), m_Class->m_MonoClass);
        mono_runtime_object_init(instance);
        m_Handle = mono_gchandle_new(instance, false);
    }

    MObject::~MObject()
    {
        mono_gchandle_free(m_Handle);
    }

    MClass &MObject::GetClass()
    {
        return *m_Class;
    }

    void MObject::Invoke(MMethod &method, void **params)
    {
        BeeExpects(method.m_MonoMethod != nullptr);
        //BeeExpects(method.m_Class == m_Class);
        MonoObject* instance = mono_gchandle_get_target(m_Handle);
        MonoObject *exception = nullptr;
        mono_runtime_invoke(method.m_MonoMethod, instance, params, &exception);
        if(exception)
        {
            mono_print_unhandled_exception(exception);
        }
    }

    void MObject::SetFieldValue(MField &field, void* value)
    {
        MonoObject* instance = mono_gchandle_get_target(m_Handle);
        mono_field_set_value(instance, field.m_MonoField, value);
    }

    bool MObject::GetFieldValue(MField &field, void* value)
    {
        MonoObject* instance = mono_gchandle_get_target(m_Handle);
        mono_field_get_value(instance, field.m_MonoField, value);
        return true;
    }
    String MObject::GetFieldStringValue(MField &field)
    {
        BeeCoreAssert(field.m_Type == MType::String, "Field is not a string!");
        MonoObject* instance = mono_gchandle_get_target(m_Handle);
        auto* str = reinterpret_cast<MonoString*>(mono_field_get_value_object(mono_domain_get(), field.m_MonoField, instance));
        String value = mono_string_to_utf8(str);
        mono_free(str);
        return value;
    }

    MonoObject *MObject::GetMonoObject()
    {
        return mono_gchandle_get_target(m_Handle);;
    }

    MObject::MObject(MonoObject *object)
    {
        m_Handle = mono_gchandle_new(object, false);
    }

    MClass::MClass(const String &name, const String &ns, MonoImage* image)
            : m_Name(name), m_Namespace(ns)
    {
        m_MonoClass = mono_class_from_name(image, m_Namespace.c_str(), m_Name.c_str());

        /*//Retrieve methods from class
        void* iterator = nullptr;

        while (MonoMethod* method = mono_class_get_methods(m_MonoClass, &iterator))
        {
            const char* methodName = mono_method_get_name(method);
            uint32_t flags = mono_method_get_flags(method, nullptr);
            MVisibility visibility = MUtils::MonoMethodFlagsToVisibility(flags);
        }*/


        // This routine is an iterator routine for retrieving the fields in a class.
        // You must pass a gpointer that points to zero and is treated as an opaque handle
        // to iterate over all of the elements. When no more values are available, the return value is NULL.

        int fieldCount = mono_class_num_fields(m_MonoClass);
        //BeeCoreTrace("{} has {} fields:", m_Name, fieldCount);
        void* iterator = nullptr;
        m_IsEnum = mono_class_is_enum(m_MonoClass);
        m_IsValueType = mono_class_is_valuetype(m_MonoClass);
        if(m_IsEnum)
            return;
        m_Fields.reserve(fieldCount);
        while (MonoClassField* field = mono_class_get_fields(m_MonoClass, &iterator))
        {
            const char* fieldName = mono_field_get_name(field);
            uint32_t flags = mono_field_get_flags(field);
            MVisibility visibility = MUtils::MonoFieldFlagsToVisibility(flags);
            bool isStatic = flags & FIELD_ATTRIBUTE_STATIC;

            MonoType* type = mono_field_get_type(field);
            MType fieldType = MUtils::MonoTypeToMType(type);
            //BeeCoreTrace("  {} ({})", fieldName, MUtils::MTypeToString(fieldType));
            if(fieldType == MType::None)
                continue;
            m_Fields.emplace(fieldName, MField(*this, fieldName, fieldType, visibility, field, isStatic));
        }
    }

    MObject MClass::Instantiate()
    {
        return {*this};
    }

    MMethod &MClass::GetMethod(const String &name, int paramCount)
    {
        if(!m_Methods.contains(name))
            m_Methods.emplace(name, MMethod(*this, name, paramCount));
        return m_Methods.at(name);
    }

    bool MClass::IsDerivedFrom(MClass &other) const
    {
        return mono_class_is_subclass_of(m_MonoClass, other.m_MonoClass, false);
    }

    MField &MClass::GetField(const String &name)
    {
        return m_Fields.at(name);
    }


    MMethod::MMethod(MClass &mClass, const String &name, int paramCount)
    : m_Class(&mClass), m_Name(name), m_ParamCount(paramCount)
    {
        m_MonoMethod = mono_class_get_method_from_name(mClass.m_MonoClass, name.c_str(), paramCount);
    }

    MMethod::~MMethod()
    {

    }

    GameScript::GameScript(MClass& mClass, Entity entity, const String& locale)
    : m_Instance(mClass.Instantiate())
    {
        //auto& constructor = ScriptingEngine::GetEntityClass().GetMethod(".ctor", 1);
        //auto uuid = entity.GetUUID();
        //void* params[1] {&uuid};
        //m_Instance.Invoke(constructor, params);
        auto& onCreate = mClass.GetMethod("OnCreate", 0);
        if(onCreate.IsValid())
            m_OnCreate = (OnFunction)mono_method_get_unmanaged_thunk(onCreate);
        auto& onDestroy = mClass.GetMethod("OnDestroy", 0);
        if(onDestroy.IsValid())
            m_OnDestroy = (OnFunction)mono_method_get_unmanaged_thunk(onDestroy);
        auto& onUpdate = mClass.GetMethod("OnUpdate", 0);
        if(onUpdate.IsValid())
            m_OnUpdate = (OnFunction)mono_method_get_unmanaged_thunk(onUpdate);

        if(entity.HasComponent<ScriptComponent>())
        {
            auto& sc = entity.GetComponent<ScriptComponent>();
            CopyFieldsData(sc.EditableFields, locale);
        }
    }

    void GameScript::InvokeOnCreate()
    {
        if(!m_OnCreate)
            return;
        MonoException* exc = nullptr;
        m_OnCreate(m_Instance.GetMonoObject(), &exc);
        if(exc)
        {
            MonoString* msg = mono_object_to_string(reinterpret_cast<MonoObject*>(exc), nullptr);
            char* message = mono_string_to_utf8(msg);
            BeeCoreError("Exception in script: {}", message);
            mono_free(message);
        }
        m_OnCreate = nullptr;
    }

    void GameScript::InvokeOnDestroy()
    {
        if(!m_OnDestroy)
            return;
        MonoException *exc = nullptr;
        m_OnDestroy(m_Instance.GetMonoObject(), &exc);
        if(exc)
        {
            MonoString* msg = mono_object_to_string(reinterpret_cast<MonoObject*>(exc), nullptr);
            char* message = mono_string_to_utf8(msg);
            BeeCoreError("Exception in script: {}", message);
            mono_free(message);
        }
    }

    void GameScript::InvokeOnUpdate()
    {
        if(!m_OnUpdate)
            return;
        MonoException *exc = nullptr;
        m_OnUpdate(m_Instance.GetMonoObject(), &exc);
        if(exc)
        {
            MonoString* msg = mono_object_to_string(reinterpret_cast<MonoObject*>(exc), nullptr);
            char* message = mono_string_to_utf8(msg);
            BeeCoreError("Exception in script: {}", message);
            mono_free(message);
        }
    }

    static MType AssetTypeToMType(AssetType type)
    {
        switch(type)
        {
            case AssetType::Texture2D:
                return MType::Texture2D;
            case AssetType::Font:
                return MType::Font;
            default:
                return MType::Asset;
        }
    }

    void GameScript::CopyFieldsData(std::vector<GameScriptField> &aClass, const String& locale)
    {
        for(auto& field : aClass)
        {
            auto& mField = field.GetMField();
            auto type = mField.GetType();
            if(type == MType::Asset || type == MType::Texture2D || type == MType::Font)
            {
                AssetHandle handle = field.GetData<AssetHandle>();
                if(AssetManager::IsAssetHandleValid(handle))
                {
                    if(type == MType::Asset)
                        type = AssetTypeToMType(AssetManager::GetAsset<Asset>(handle, locale).GetType());
                    ScriptingEngine::SetAssetHandle(m_Instance, mField, handle, type);
                }
                continue;
            }
            m_Instance.SetFieldValue(mField, field.GetData());
        }
    }
}