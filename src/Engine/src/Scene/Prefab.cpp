//
// Created by alexl on 16.10.2023.
//

#include "Prefab.h"

namespace BeeEngine
{
    Ref<Scene> Prefab::s_PrefabScene = nullptr;
    constexpr AssetType Prefab::GetType() const
    {
        return AssetType::Prefab;
    }

    Prefab::~Prefab()
    {
        if (m_RootEntity && IsPrefabSceneInitialized())
            GetPrefabScene()->DestroyEntity(m_RootEntity);
    }
} // namespace BeeEngine