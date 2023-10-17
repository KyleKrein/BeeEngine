//
// Created by alexl on 16.10.2023.
//

#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/TypeDefines.h"
#include "Scene.h"
#include "Entity.h"

namespace BeeEngine
{
    class Entity;
    class Prefab final: public Asset
    {
        friend Scene;
        friend class PrefabImporter;
    public:
        [[nodiscard]] constexpr AssetType GetType() const final;
        ~Prefab() final;
    private:
        static Ref<Scene>& GetPrefabScene()
        {
            static Ref<Scene> scene = CreateRef<Scene>();
            return scene;
        }
        Entity m_RootEntity = Entity::Null;
    };
}
