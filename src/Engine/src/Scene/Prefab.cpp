//
// Created by alexl on 16.10.2023.
//

#include "Prefab.h"


namespace BeeEngine
{
    constexpr AssetType Prefab::GetType() const
    {
        return AssetType::Prefab;
    }

    Prefab::~Prefab()
    {
        if(m_RootEntity)
            GetPrefabScene()->DestroyEntity(m_RootEntity);
    }
}