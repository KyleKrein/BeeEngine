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


namespace BeeEngine
{
    MAssembly::MAssembly(const std::filesystem::path& path)
    : m_Path(path)
    {
        LoadAssembly();
        GetClassesFromAssembly();
        mono_image_close(m_MonoImage);
        m_MonoImage = nullptr;
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

            const char* name = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAME]);
            const char* ns = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAMESPACE]);

            m_Classes.emplace_back(MClass(name, ns));
        }
    }

    void MAssembly::Reload()
    {
        UnloadAssembly();
        LoadAssembly();
        GetClassesFromAssembly();
        mono_image_close(m_MonoImage);
        m_MonoImage = nullptr;
    }

    void MAssembly::UnloadAssembly()
    {
        mono_assembly_close(m_MonoAssembly);
        m_MonoAssembly = nullptr;
    }
}