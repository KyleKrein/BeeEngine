//
// Created by alexl on 16.08.2023.
//

#include "EditorAssetManager.h"
#include "AssetImporter.h"
#include "Core/ResourceManager.h"
#include "EngineAssetRegistry.h"

namespace BeeEngine
{

    Ref<Asset> EditorAssetManager::GetAssetRef(AssetHandle handle) const
    {
        BeeExpects(IsAssetHandleValid(handle));
        if (!IsAssetLoaded(handle))
        {
            const auto& metadata = m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
            m_AssetMap[handle] = AssetImporter::ImportAsset(handle, metadata);
            m_AssetMap.at(handle)->Name = metadata.Name;
        }
        return m_AssetMap.at(handle);
    }

    void
    EditorAssetManager::LoadAsset(gsl::span<byte> data, AssetHandle handle, const std::string& name, AssetType type)
    {
        BeeExpects(!IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
        AssetMetadata metadata;
        metadata.Name = name;
        metadata.Type = type;
        metadata.Location = AssetLocation::Embedded;
        metadata.Data = data;
        m_AssetRegistry[handle.RegistryID][handle.AssetID] = metadata;
        BeeEnsures(IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
    }

    void EditorAssetManager::LoadAsset(const Path& path, AssetHandle handle)
    {
        // BeeExpects(!IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
        if (IsAssetHandleValid(handle) && IsAssetLoaded(handle))
        {
            auto& metadata = m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
            if (path != std::get<Path>(metadata.Data))
            {
                metadata.Data = path;
                metadata.Name = path.GetFileNameWithoutExtension();
            }
            UnloadAsset(handle);
            return;
        }
        AssetMetadata metadata;
        metadata.Data = path;
        metadata.Name = path.GetFileNameWithoutExtension();
        metadata.Type = ResourceManager::GetAssetTypeFromExtension(path.GetExtension());
        metadata.Location = AssetLocation::FileSystem;
        m_AssetRegistry[handle.RegistryID][handle.AssetID] = metadata;
        m_AssetNameMap[metadata.Name] = handle;
        BeeEnsures(IsAssetHandleValid(handle));

        BeeCoreTrace("Loaded asset: {0}", metadata.Name);
    }

    void EditorAssetManager::UnloadAsset(AssetHandle handle)
    {
        BeeExpects(IsAssetHandleValid(handle) && IsAssetLoaded(handle));
        m_AssetMap.erase(handle);
        BeeEnsures(!IsAssetLoaded(handle));
    }

    bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
    {
        return m_AssetRegistry.contains(handle.RegistryID) &&
               m_AssetRegistry.at(handle.RegistryID).contains(handle.AssetID);
    }

    bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_AssetMap.contains(handle);
    }

    Asset* EditorAssetManager::GetAsset(AssetHandle handle) const
    {
        BeeExpects(IsAssetHandleValid(handle));
        if (!IsAssetLoaded(handle))
        {
            const AssetMetadata& metadata = m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
            m_AssetMap[handle] = AssetImporter::ImportAsset(handle, metadata);
            m_AssetMap.at(handle)->Name = metadata.Name;
        }
        return m_AssetMap.at(handle).get();
    }

    AssetMetadata& EditorAssetManager::GetAssetMetadata(const AssetHandle& handle)
    {
        BeeExpects(IsAssetHandleValid(handle));
        return m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
    }

    const AssetHandle* EditorAssetManager::GetAssetHandleByName(std::string_view name) const
    {
        if (!m_AssetNameMap.contains(name.data()))
        {
            return nullptr;
        }
        return &m_AssetNameMap.at(name.data());
    }

    EditorAssetManager::EditorAssetManager()
    {
        AssetManager::s_AssetManager = this;
        EngineAssetRegistry::RegisterAssetTypes(this);
    }

    void EditorAssetManager::RemoveAsset(AssetHandle handle)
    {
        BeeExpects(IsAssetHandleValid(handle));
        if (IsAssetLoaded(handle))
        {
            UnloadAsset(handle);
        }
        auto metadata = m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
        m_AssetRegistry.at(handle.RegistryID).erase(handle.AssetID);
        m_AssetNameMap.erase(metadata.Name);
        /*if(m_AssetRegistry.at(handle.RegistryID).empty()) //Commented out because deleting of last element in
        project's asset registry makes it impossible to save registry correctly
        {
            m_AssetRegistry.erase(handle.RegistryID);
        }*/
    }
} // namespace BeeEngine
