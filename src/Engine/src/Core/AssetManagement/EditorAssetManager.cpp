//
// Created by alexl on 16.08.2023.
//

#include "EditorAssetManager.h"
#include "AssetImporter.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetMetadata.h"
#include "Core/ResourceManager.h"
#include "EngineAssetRegistry.h"
#include "RmlUi/Core/Core.h"

namespace BeeEngine
{
    [[nodiscard]] Ref<Asset> EditorAssetManager::GetAssetRef(const Path& path)
    {
        bool hasExtension = !path.GetExtension().IsEmpty();
        auto name = path.GetFileNameWithoutExtension().AsUTF8();
        auto* handlePtr = GetAssetHandleByName(name);
        if (!handlePtr)
        {
            if (m_EditedRegistryID == 0)
            {
                BeeCoreWarn("Trying to load not registered asset {} . If you want to edit asset registry, call "
                            "SetEditedAssetRegistryID() with your AssetRegistry ID. Note: it's not for production, "
                            "only for development",
                            path);
                return nullptr;
            }
            AssetHandle handle = {m_EditedRegistryID};
            LoadAsset(path, handle);
            return GetAssetRef(handle);
        }
        auto handle = *handlePtr;
        if (hasExtension)
        {
            AssetType type = ResourceManager::GetAssetTypeFromExtension(path.GetExtension());
            BeeExpects(GetAssetMetadata(handle).Type == type &&
                       "There should be only one file with this filename. (Unique name needed)");
        }
        return GetAssetRef(handle);
    }
    Asset* EditorAssetManager::GetAsset(const Path& path)
    {
        return GetAssetRef(path).get();
    }

    Ref<Asset> EditorAssetManager::GetAssetRef(AssetHandle handle) const
    {
        BeeExpects(IsAssetHandleValid(handle));
        if (!IsAssetLoaded(handle))
        {
            const auto& metadata = m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
            m_AssetMap[handle] = AssetImporter::ImportAsset(handle, metadata);
            m_AssetMap.at(handle)->Name = static_cast<std::string_view>(metadata.Name);
        }
        return m_AssetMap.at(handle);
    }

    void EditorAssetManager::LoadAsset(std::span<byte> data, AssetHandle handle, const String& name, AssetType type)
    {
        BeeExpects(!IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
        if (type == AssetType::Font)
        {
            Rml::LoadFontFace(Rml::Span<const Rml::byte>{(Rml::byte*)data.data(), data.size()},
                              "",
                              Rml::Style::FontStyle::Normal, // Assuming that it's always not Italic :/
                              Rml::Style::FontWeight::Auto,
                              true);
        }
        AssetMetadata metadata;
        metadata.Name = name;
        metadata.Type = type;
        metadata.Location = AssetLocation::Embedded;
        metadata.Data = data;
        m_TypeMap[type].push_back(handle);
        m_AssetNameMap[name] = handle;
        m_AssetRegistry[handle.RegistryID][handle.AssetID] = metadata;
        BeeEnsures(IsAssetHandleValid(handle) && !IsAssetLoaded(handle));
    }

    void EditorAssetManager::LoadAsset(const Path& path, AssetHandle handle)
    {
        if (IsAssetHandleValid(handle) && IsAssetLoaded(handle))
        {
            auto& metadata = m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
            if (path != std::get<Path>(metadata.Data))
            {
                metadata.Data = path;
                m_AssetNameMap.erase(metadata.Name);
                metadata.Name = path.GetFileNameWithoutExtension();
                m_AssetNameMap[metadata.Name] = handle;
                if (metadata.Type == AssetType::Font)
                {
                    Rml::LoadFontFace(path.AsCString(), true);
                }
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
        m_TypeMap[metadata.Type].push_back(handle);
        if (metadata.Type == AssetType::Font)
        {
            Rml::LoadFontFace(path.AsCString(), true);
        }

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
            m_AssetMap.at(handle)->Name = static_cast<std::string_view>(metadata.Name);
        }
        return m_AssetMap.at(handle).get();
    }

    AssetMetadata& EditorAssetManager::GetAssetMetadata(const AssetHandle& handle)
    {
        BeeExpects(IsAssetHandleValid(handle));
        return m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID);
    }

    const AssetHandle* EditorAssetManager::GetAssetHandleByName(const String& name) const
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
        auto it = std::find(m_TypeMap[metadata.Type].begin(), m_TypeMap[metadata.Type].end(), handle);
        if (it != m_TypeMap[metadata.Type].end())
        {
            m_TypeMap[metadata.Type].erase(it);
        }
        /*if(m_AssetRegistry.at(handle.RegistryID).empty()) //Commented out because deleting of last element in
        project's asset registry makes it impossible to save registry correctly
        {
            m_AssetRegistry.erase(handle.RegistryID);
        }*/
    }

    std::span<const AssetHandle> EditorAssetManager::GetAssetHandlesByType(AssetType type) const
    {
        if (m_TypeMap.contains(type))
        {
            return m_TypeMap.at(type);
        }
        return {};
    }

    Generator<Asset&> EditorAssetManager::GetAssetsOfType(AssetType type) const
    {
        if (m_TypeMap.contains(type))
        {
            for (const auto& handle : m_TypeMap.at(type))
            {
                co_yield *GetAsset(handle);
            }
        }
    }

    Generator<Asset&> EditorAssetManager::IterateAssets() const
    {
        for (const auto& [registryID, registryMap] : m_AssetRegistry)
        {
            for (const auto& [assetID, metadata] : registryMap)
            {
                co_yield *GetAsset({registryID, assetID});
            }
        }
    }

    Generator<std::pair<AssetHandle, const AssetMetadata*>>
    EditorAssetManager::GetAssetsDataOfType(AssetType type) const
    {
        if (m_TypeMap.contains(type))
        {
            for (const auto& handle : m_TypeMap.at(type))
            {
                co_yield std::make_pair(handle, &m_AssetRegistry.at(handle.RegistryID).at(handle.AssetID));
            }
        }
    }

    Generator<std::pair<AssetHandle, const AssetMetadata*>> EditorAssetManager::IterateAssetsData() const
    {
        for (const auto& [registryID, registryMap] : m_AssetRegistry)
        {
            for (const auto& [assetID, metadata] : registryMap)
            {
                co_yield std::make_pair(AssetHandle{registryID, assetID}, &metadata);
            }
        }
    }
} // namespace BeeEngine
