//
// Created by Александр Лебедев on 22.10.2023.
//

#pragma once
#include "Asset.h"
#include "AssetMetadata.h"
#include "Core/TypeDefines.h"
#include "Serialization/Serializable.h"
#include <unordered_map>
namespace BeeEngine
{

    class LocalizedAsset : public Asset
    {
        friend class LocalizedAssetSerializer;
        using Locale = UTF8String;

    public:
        LocalizedAsset(std::initializer_list<std::pair<Locale, AssetHandle>> assets)
        {
            for (auto& asset : assets)
            {
                m_Assets[asset.first] = asset.second;
            }
        }
        LocalizedAsset(std::unordered_map<Locale, AssetHandle>&& assets) : m_Assets(std::move(assets)) {}
        [[nodiscard]] constexpr AssetType GetType() const override { return AssetType::Localized; }
        [[nodiscard]] Ref<Asset> GetAssetRef(const Locale& locale) const;
        [[nodiscard]] Asset& GetAsset(const Locale& locale) const;

        template <typename Archive>
        void Serialize(Archive& archive)
        {
            // archive & Serialization::Key{"Name"} & Serialization::Value{Name};
            // archive & Serialization::Key{};
        }

        template <typename Archive>
        void Deserialize(Archive& archive)
        {
        }

    private:
        std::unordered_map<Locale, AssetHandle> m_Assets;
        mutable Locale m_CurrentLocale;
        mutable Ref<Asset> m_CachedAsset = nullptr;
    };

    class LocalizedAssetImporter
    {
    public:
        static Ref<LocalizedAsset> ImportLocalizedAsset(AssetHandle handle, const AssetMetadata& metadata);
    };

    class LocalizedAssetSerializer
    {
    public:
        static String Serialize(const LocalizedAsset& asset);
        static Ref<LocalizedAsset> Deserialize(const String& data);
    };
} // namespace BeeEngine
