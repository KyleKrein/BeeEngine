//
// Created by alexl on 16.10.2023.
//

#include "PrefabImporter.h"
#include "Core/String.h"
#include "FileSystem/File.h"
#include "Scene/Components.h"
#include "Scene/Prefab.h"
#include "Scene/SceneSerializer.h"

namespace BeeEngine
{

    Ref<Asset> PrefabImporter::ImportPrefab(AssetHandle handle, const AssetMetadata& metadata)
    {
        auto prefab = CreateRef<Prefab>();
        String prefabFile;
        if (metadata.Location == AssetLocation::FileSystem)
        {
            prefabFile = File::ReadFile(std::get<Path>(metadata.Data));
            if (prefabFile.empty())
            {
                BeeCoreError("Failed to read prefab file: {}", std::get<Path>(metadata.Data));
                return nullptr;
            }
        }
        else if (metadata.Location == AssetLocation::Embedded)
        {
            auto& data = std::get<gsl::span<byte>>(metadata.Data);
            prefabFile = String((char*)data.data(), data.size());
        }
        else
        {
            BeeCoreError("Unknown asset location: {}", metadata.Location);
            return nullptr;
        }
        SceneSerializer serializer(prefab->GetPrefabScene());
        prefab->m_RootEntity = serializer.DeserializeEntityFromString(prefabFile);
        return prefab;
    }

    Ref<Asset> PrefabImporter::GeneratePrefab(Entity rootEntity, const Path& path, const AssetHandle& handle)
    {
        BeeExpects(rootEntity);
        Entity prefabEntity;
        Ref<Scene> tempScene = CreateRef<Scene>();
        if (rootEntity.HasParent())
        {
            Entity temp = rootEntity.GetParent();
            rootEntity.GetComponent<HierarchyComponent>().Parent = Entity::Null;
            prefabEntity = rootEntity.m_Scene->CopyEntity(rootEntity, *tempScene, Entity::Null, false);
            rootEntity.GetComponent<HierarchyComponent>().Parent = temp;
        }
        else
            prefabEntity = rootEntity.m_Scene->CopyEntity(rootEntity, *tempScene, Entity::Null, false);
        SceneSerializer serializer(tempScene);
        auto serialized = serializer.SerializeEntityToString(prefabEntity);
        File::WriteFile(path, serialized);
        AssetManager::LoadAsset(path, handle);
        return AssetManager::GetAssetRef<Prefab>(handle);
    }
} // namespace BeeEngine
