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
        static Ref<T> GetAsset(AssetHandle handle)
        {
            BeeExpects(s_AssetManager);
            return std::dynamic_pointer_cast<T>(s_AssetManager->GetAsset(handle));
        }

    private:
        static IAssetManager* s_AssetManager;
        friend class Application;
    };
}
