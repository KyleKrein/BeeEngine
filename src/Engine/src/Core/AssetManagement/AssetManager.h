//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "Asset.h"
#include "Core/TypeDefines.h"
#include "IAssetManager.h"
#include "Locale/Locale.h"
#include "LocalizedAsset.h"
#include <type_traits>
#include <utility>
#include <span>

namespace BeeEngine
{
    class Texture2D;
    class Font;
    class AssetManager
    {
    public:
        template <typename T>
            requires std::is_same_v<T, Font> || std::is_same_v<T, Texture2D> || std::is_same_v<T, Asset>
        static Ref<T> GetAssetRef(const AssetHandle& handle, const Locale::Localization& locale)
        {
            BeeExpects(s_AssetManager);
            Ref<Asset> asset = s_AssetManager->GetAssetRef(handle);
            if (asset->GetType() == AssetType::Localized)
            {
                return std::dynamic_pointer_cast<T>(
                    std::static_pointer_cast<LocalizedAsset>(asset)->GetAssetRef(locale.GetLanguageString()));
            }
            return std::dynamic_pointer_cast<T>(asset);
        }
        template <typename T>
            requires std::is_same_v<T, Texture2D> || std::is_same_v<T, Font> || std::is_same_v<T, Asset>
        static T& GetAsset(const AssetHandle& handle, const Locale::Localization& locale)
        {
            BeeExpects(s_AssetManager);
            Asset* asset = s_AssetManager->GetAsset(handle);
            if (asset->GetType() == AssetType::Localized)
            {
                return dynamic_cast<T&>(dynamic_cast<LocalizedAsset*>(asset)->GetAsset(locale.GetLanguageString()));
            }
            return dynamic_cast<T&>(*asset);
        }
        template <typename T>
            requires(std::is_base_of_v<Asset, T> || std::is_same_v<Asset, T>) && (!std::is_same_v<T, Texture2D>) &&
                    (!std::is_same_v<T, Font>)
        static Ref<T> GetAssetRef(const AssetHandle& handle)
        {
            BeeExpects(s_AssetManager);
            return std::dynamic_pointer_cast<T>(s_AssetManager->GetAssetRef(handle));
        }
        template <typename T>
            requires(std::is_base_of_v<Asset, T> || std::is_same_v<Asset, T>) && (!std::is_same_v<T, Texture2D>) &&
                    (!std::is_same_v<T, Font>)
        static T& GetAsset(const AssetHandle& handle)
        {
            BeeExpects(s_AssetManager);
            return dynamic_cast<T&>(*s_AssetManager->GetAsset(handle));
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
        static void LoadAsset(std::span<byte> data, AssetHandle handle, const String& name, AssetType type)
        {
            BeeExpects(s_AssetManager);
            s_AssetManager->LoadAsset(data, std::move(handle), name, type);
        }
        static void LoadAsset(const Path& path, AssetHandle handle)
        {
            BeeExpects(s_AssetManager);
            s_AssetManager->LoadAsset(path, std::move(handle));
        }

    private:
        static IAssetManager* s_AssetManager;
        friend class Application;
        friend class Layer;
        friend class EditorAssetManager;
    };
} // namespace BeeEngine
