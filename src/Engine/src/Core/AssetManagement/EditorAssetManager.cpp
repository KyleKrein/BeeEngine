//
// Created by alexl on 16.08.2023.
//

#include "EditorAssetManager.h"
#include "Core/ResourceManager.h"
#include "AssetImporter.h"

namespace BeeEngine
{

    Ref<Asset> EditorAssetManager::GetAssetRef(AssetHandle handle) const
    {
        BeeExpects(IsAssetHandleValid(handle));
        if(!IsAssetLoaded(handle))
        {
            const auto& metadata = m_AssetRegistry.at(handle);
            m_AssetMap[handle] = AssetImporter::ImportAsset(handle, metadata);
        }
        return m_AssetMap.at(handle);
    }

    void EditorAssetManager::LoadAsset(gsl::span<byte> data, AssetHandle handle, const std::string& name, AssetType type)
    {
        BeeExpects(!IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
        AssetMetadata metadata;
        metadata.Name = name;
        metadata.Type = type;
        metadata.Location = AssetLocation::Embedded;
        metadata.Data = data;
        m_AssetRegistry[handle] = metadata;
        BeeEnsures(IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
    }

    void EditorAssetManager::LoadAsset(const std::filesystem::path &path, AssetHandle handle)
    {
        BeeExpects(!IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
        AssetMetadata metadata;
        metadata.Data = path;
        std::string pathStr = path.string();
        metadata.Name = ResourceManager::GetNameFromFilePath(pathStr);
        metadata.Type = ResourceManager::GetAssetTypeFromFilePath(pathStr);
        metadata.Location = AssetLocation::FileSystem;
        m_AssetRegistry[handle] = metadata;
        m_AssetNameMap[metadata.Name] = handle;
        BeeEnsures(IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
    }

    void EditorAssetManager::UnloadAsset(AssetHandle handle)
    {
        BeeExpects(IsAssetHandleValid(handle) && IsAssetLoaded(handle));
        m_AssetMap.erase(handle);
        BeeEnsures(!IsAssetLoaded(handle));
    }

    bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
    {
        return m_AssetRegistry.contains(handle);
    }

    bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_AssetMap.contains(handle);
    }

    Asset * EditorAssetManager::GetAsset(AssetHandle handle) const
    {
        BeeExpects(IsAssetHandleValid(handle));
        if(!IsAssetLoaded(handle))
        {
            const auto& metadata = m_AssetRegistry.at(handle);
            m_AssetMap[handle] = AssetImporter::ImportAsset(handle, metadata);
        }
        return m_AssetMap.at(handle).get();
    }

    const AssetHandle* EditorAssetManager::GetAssetHandleByName(std::string_view name) const
    {
        if(!m_AssetNameMap.contains(name.data()))
        {
            return nullptr;
        }
        return &m_AssetNameMap.at(name.data());
    }
}
