//
// Created by alexl on 16.08.2023.
//

#pragma once
#include <type_traits>
#include "Asset.h"
#include "Core/TypeDefines.h"
#include "IAssetManager.h"

namespace BeeEngine
{
    class AssetManager
    {
    public:
        template<typename T>
        requires std::is_base_of_v<Asset, T>
        static Ref<T> GetAssetRef(const AssetHandle& handle)
        {
            BeeExpects(s_AssetManager);
            return std::dynamic_pointer_cast<T>(s_AssetManager->GetAssetRef(handle));
        }
        template<typename T>
        requires std::is_base_of_v<Asset, T>
        static T& GetAsset(const AssetHandle& handle)
        {
            BeeExpects(s_AssetManager);
            return *(T*)s_AssetManager->GetAsset(handle);
        }

        static bool IsAssetHandleValid(const AssetHandle& handle)
        {
            BeeExpects(s_AssetManager);
            return s_AssetManager->IsAssetHandleValid(handle);
        }

        static bool IsAssetLoaded(const AssetHandle& handle)
        {
            BeeExpects(s_AssetManager);
            return s_AssetManager->IsAssetLoaded(handle);
        }

        static void UnloadAsset(const AssetHandle& handle)
        {
            BeeExpects(s_AssetManager);
            s_AssetManager->UnloadAsset(handle);
        }

    private:
        static IAssetManager* s_AssetManager;
        friend class Application;
        friend class Layer;
        friend class EditorAssetManager;
    };
}