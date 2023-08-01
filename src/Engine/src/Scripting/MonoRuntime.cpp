//
// Created by alexl on 30.07.2023.
//
#include "MAssembly.h"
#include "MClass.h"
#include "Utils/File.h"
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


namespace BeeEngine
{
    MAssembly::MAssembly(const std::filesystem::path& path)
    : m_Path(path)
    {
        LoadAssembly();
        mono_image_close(m_MonoImage);
        m_MonoImage = mono_assembly_get_image(m_MonoAssembly);
        GetClassesFromAssembly();
    }
    MAssembly::~MAssembly()
    {
        UnloadAssembly();
    }
    void MAssembly::LoadAssembly()
    {
        auto fileData = File::ReadBinaryFile(m_Path.string());

        MonoImageOpenStatus status;
        m_MonoImage = mono_image_open_from_data_full(reinterpret_cast<char *>(fileData.data()), fileData.size(), true,
                                                     &status, false);
        BeeCoreAssert(status == MONO_IMAGE_OK, "Failed to load assembly! Status: {}", mono_image_strerror(status));
        m_MonoAssembly = mono_assembly_load_from_full(m_MonoImage, m_Path.string().c_str(), &status, false);
    }

    std::vector<MClass>& MAssembly::GetClasses()
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
            if(name.contains("<Module>") || name.contains(anType))
                continue;
            const char* ns = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAMESPACE]);

            m_Classes.emplace_back(MClass(name, ns, m_MonoImage));
        }
    }

    void MAssembly::Reload()
    {
        UnloadAssembly();
        LoadAssembly();
        mono_image_close(m_MonoImage);
        m_MonoImage = mono_assembly_get_image(m_MonoAssembly);
        GetClassesFromAssembly();
        m_MonoImage = nullptr;
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
        m_MonoObject = mono_object_new(mono_domain_get(), m_Class->m_MonoClass);
        mono_runtime_object_init(m_MonoObject);
    }

    MObject::~MObject()
    {

    }

    MClass &MObject::GetClass()
    {
        return *m_Class;
    }

    void MObject::Invoke(MMethod &method, void **params)
    {
        BeeExpects(method.m_MonoMethod != nullptr);
        //BeeExpects(method.m_Class == m_Class);
        MonoObject *exception = nullptr;
        mono_runtime_invoke(method.m_MonoMethod, m_MonoObject, params, &exception);
        if(exception)
        {
            mono_print_unhandled_exception(exception);
            mono_free(exception);
        }
    }

    void MObject::SetFieldValue(MField &field, void* value)
    {
        mono_field_set_value(m_MonoObject, field.m_MonoField, value);
    }

    bool MObject::GetFieldValue(MField &field, void* value)
    {
        mono_field_get_value(m_MonoObject, field.m_MonoField, value);
        return true;
    }
    String MObject::GetFieldStringValue(MField &field)
    {
        BeeCoreAssert(field.m_Type == MType::String, "Field is not a string!");
        auto* str = reinterpret_cast<MonoString*>(mono_field_get_value_object(mono_domain_get(), field.m_MonoField, m_MonoObject));
        String value = mono_string_to_utf8(str);
        mono_free(str);
        return value;
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
        BeeCoreTrace("{} has {} fields:", m_Name, fieldCount);
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
            BeeCoreTrace("  {} ({})", fieldName, MUtils::MTypeToString(fieldType));
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

    GameScript::GameScript(MClass& mClass, Entity entity)
    : m_Instance(mClass.Instantiate())
    {
        auto& constructor = ScriptingEngine::GetEntityClass().GetMethod(".ctor", 1);
        auto uuid = entity.GetUUID();
        void* params[1] {&uuid};
        m_Instance.Invoke(constructor, params);
        m_OnCreate = &mClass.GetMethod("OnCreate", 0);
        if(!m_OnCreate->IsValid())
            m_OnCreate = nullptr;
        m_OnDestroy = &mClass.GetMethod("OnDestroy", 0);
        if(!m_OnDestroy->IsValid())
            m_OnDestroy = nullptr;
        m_OnUpdate = &mClass.GetMethod("OnUpdate", 0);
        if(!m_OnUpdate->IsValid())
            m_OnUpdate = nullptr;


        SelectEditableFields(mClass);
    }

    void GameScript::InvokeOnCreate()
    {
        if(m_OnCreate)
            m_Instance.Invoke(*m_OnCreate, nullptr);
    }

    void GameScript::InvokeOnDestroy()
    {
        if(m_OnDestroy)
        {
            m_Instance.Invoke(*m_OnDestroy, nullptr);
            m_OnDestroy = nullptr;
        }
    }

    void GameScript::InvokeOnUpdate()
    {
        if(m_OnUpdate)
            m_Instance.Invoke(*m_OnUpdate, nullptr);
    }

    void GameScript::SelectEditableFields(MClass &aClass)
    {
        auto& fields = aClass.GetFields();
        for(auto& [name, field] : fields)
        {
            if(MUtils::IsSutableForEdit(field))
            {
                m_EditableFields.emplace(name, &field);
            }
        }
    }
}