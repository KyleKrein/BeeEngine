//
// Created by Александр Лебедев on 22.10.2023.
//

#include "LocalizedAsset.h"
#include "AssetManager.h"
#include <yaml-cpp/yaml.h>
#include "FileSystem/File.h"

namespace YAML
{
    template<>
    struct convert<BeeEngine::AssetHandle>
    {
        static Node encode(const BeeEngine::AssetHandle& rhs);

        static bool decode(const Node& node, BeeEngine::AssetHandle& rhs);
    };
}
namespace BeeEngine
{
    YAML::Emitter& operator<<(YAML::Emitter& out, const AssetHandle& handle);
    Ref<Asset> LocalizedAsset::GetAssetRef(const LocalizedAsset::Locale &locale) const
    {
        if(m_CurrentLocale != locale || !m_CachedAsset)
        {
            m_CurrentLocale = locale;
            m_CachedAsset = AssetManager::GetAssetRef<Asset>(m_Assets.at(locale), m_CurrentLocale);
        }
        return m_CachedAsset;
    }

    Asset &LocalizedAsset::GetAsset(const LocalizedAsset::Locale &locale) const
    {
        if(m_CurrentLocale != locale || !m_CachedAsset)
        {
            m_CurrentLocale = locale;
            m_CachedAsset = AssetManager::GetAssetRef<Asset>(m_Assets.at(locale), m_CurrentLocale);
        }
        return *m_CachedAsset;
    }

    Ref<LocalizedAsset> LocalizedAssetImporter::ImportLocalizedAsset(AssetHandle handle, const AssetMetadata &metadata)
    {
        if(metadata.Location == AssetLocation::FileSystem)
        {
            String content = File::ReadFile(std::get<Path>(metadata.Data));
            return LocalizedAssetSerializer::Deserialize(content);
        }
        else if(metadata.Location == AssetLocation::Embedded)
        {
            String content = String((char*)std::get<gsl::span<byte>>(metadata.Data).data(), std::get<gsl::span<byte>>(metadata.Data).size());
            return LocalizedAssetSerializer::Deserialize(content);
        }
        BeeCoreError("Unknown asset location: {}", metadata.Location);
        return nullptr;
    }

    String LocalizedAssetSerializer::Serialize(const LocalizedAsset &asset)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        for(auto& [locale, handle] : asset.m_Assets)
        {
            out << YAML::Key << locale;
            out << YAML::Value << handle;
        }
        out << YAML::EndMap;
        return out.c_str();
    }

    Ref<LocalizedAsset> LocalizedAssetSerializer::Deserialize(const String &data)
    {
        std::unordered_map<LocalizedAsset::Locale, AssetHandle> assets;
        YAML::Node node = YAML::Load(data);
        for (const auto& pair : node)
        {
            auto locale = pair.first.as<LocalizedAsset::Locale>();
            auto handle = pair.second.as<AssetHandle>();

            assets.emplace(std::move(locale), std::move(handle));
        }
        return CreateRef<LocalizedAsset>(std::move(assets));
    }
} // BeeEngine