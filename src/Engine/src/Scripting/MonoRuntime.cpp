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
#include <mono/metadata/object.h>


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

    const std::vector<MClass>& MAssembly::GetClasses()
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
        mono_assembly_close(m_MonoAssembly);
        m_MonoAssembly = nullptr;
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
        BeeExpects(method.m_Class == m_Class);
        mono_runtime_invoke(method.m_MonoMethod, m_MonoObject, params, nullptr);
    }

    MClass::MClass(const String &name, const String &ns, MonoImage* image)
            : m_Name(name), m_Namespace(ns)
    {
        m_MonoClass = mono_class_from_name(image, m_Namespace.c_str(), m_Name.c_str());
    }

    MObject MClass::Instantiate()
    {
        return {*this};
    }

    MMethod &MClass::GetMethod(const String &name, int paramCount)
    {
        if(!m_Methods.contains(name))
            m_Methods.emplace(name, MMethod(*this, name, 0));
        return m_Methods.at(name);
    }


    MMethod::MMethod(MClass &mClass, const String &name, int paramCount)
    {
        m_MonoMethod = mono_class_get_method_from_name(mClass.m_MonoClass, name.c_str(), paramCount);
    }

    MMethod::~MMethod()
    {

    }
}