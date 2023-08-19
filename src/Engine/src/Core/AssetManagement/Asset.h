//
// Created by alexl on 16.08.2023.
//

#pragma once

#include "Core/UUID.h"
namespace BeeEngine
{
    struct AssetHandle
    {
        UUID RegistryID {};
        UUID AssetID {};
        constexpr AssetHandle() = default;
        constexpr AssetHandle(UUID registryID, UUID assetID)
            : RegistryID(registryID), AssetID(assetID)
        {}
        AssetHandle(UUID registryID)
                : RegistryID(registryID)
        {}
        constexpr AssetHandle(AssetHandle&& other) noexcept
            : RegistryID(other.RegistryID), AssetID(other.AssetID)
        {}
        constexpr AssetHandle(const AssetHandle& other) noexcept
            : RegistryID(other.RegistryID), AssetID(other.AssetID)
        {}
        constexpr AssetHandle& operator=(AssetHandle&& other) noexcept
        {
            RegistryID = other.RegistryID;
            AssetID = other.AssetID;
            return *this;
        }
        constexpr AssetHandle& operator=(const AssetHandle& other) noexcept
        {
            RegistryID = other.RegistryID;
            AssetID = other.AssetID;
            return *this;
        }

        bool operator < (const AssetHandle& other) const
        {
            return RegistryID < other.RegistryID || (RegistryID == other.RegistryID && AssetID < other.AssetID);
        }
    };
    enum class AssetType: uint16_t
    {
        None = 0,
        Texture2D = 1,
        Font = 2,
        Shader = 3,
        Mesh = 4,
        Material = 5,
        Model = 6,
    };
    enum class AssetLocation: uint16_t
    {
        FileSystem = 0,
        Embedded = 1
    };
    struct Asset
    {
        AssetHandle Handle;
        AssetLocation Location = AssetLocation::FileSystem;
        std::string_view Name;
        virtual constexpr AssetType GetType() const = 0;

        virtual ~Asset() = default;
    };
}
namespace std
{
    template<>
    struct hash<BeeEngine::AssetHandle>
    {
        std::size_t operator()(const BeeEngine::AssetHandle& uuid) const
        {
            return hash<BeeEngine::UUID>()(uuid.RegistryID) / 2 + hash<BeeEngine::UUID>()(uuid.AssetID) / 2;
        }
    };

}