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
    class Application;
    class Prefab final: public Asset
    {
        friend Scene;
        friend class PrefabImporter;
        friend class Application;
    public:
        [[nodiscard]] constexpr AssetType GetType() const final;
        ~Prefab() final;
    private:
        static Ref<Scene>& GetPrefabScene()
        {
            return s_PrefabScene;
        }
        static void InitPrefabScene()
        {
            s_PrefabScene = CreateRef<Scene>();
        }
        static void ResetPrefabScene()
        {
            s_PrefabScene.reset();
        }
        static bool IsPrefabSceneInitialized()
        {
            return s_PrefabScene != nullptr;
        }
        static Ref<Scene> s_PrefabScene;
        Entity m_RootEntity = Entity::Null;
    };
}
