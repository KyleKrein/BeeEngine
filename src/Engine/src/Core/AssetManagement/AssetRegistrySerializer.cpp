//
// Created by alexl on 16.08.2023.
//

#include "AssetRegistrySerializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include "Core/ResourceManager.h"

namespace BeeEngine
{
    static const char* AssetTypeToString(AssetType type)
    {
        switch (type)
        {
            case AssetType::None:
                return "AssetType::None";
            case AssetType::Texture2D:
                return "AssetType::Texture2D";
            case AssetType::Font:
                return "AssetType::Font";
            case AssetType::Shader:
                return "AssetType::Shader";
            case AssetType::Mesh:
                return "AssetType::Mesh";
            case AssetType::Material:
                return "AssetType::Material";
            case AssetType::Model:
                return "AssetType::Model";
        }
        BeeCoreError("Unknown AssetType: {0}", ToString(type));
        return "AssetType::None";
    }

    static AssetType AssetTypeFromString(const std::string& type)
    {
        if (type == "AssetType::None")
            return AssetType::None;
        if (type == "AssetType::Texture2D")
            return AssetType::Texture2D;
        if (type == "AssetType::Font")
            return AssetType::Font;
        if (type == "AssetType::Shader")
            return AssetType::Shader;
        if (type == "AssetType::Mesh")
            return AssetType::Mesh;
        if (type == "AssetType::Material")
            return AssetType::Material;
        if (type == "AssetType::Model")
            return AssetType::Model;
        BeeCoreError("Unknown AssetType: {0}", type);
        return AssetType::None;
    }

    void AssetRegistrySerializer::Serialize(const std::filesystem::path &path)
    {
        auto& registry = m_AssetManager->GetAssetRegistry();
        if(!registry.contains(m_ProjectRegistryID))
            return;
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Registry ID" << YAML::Value << m_ProjectRegistryID;
        out << YAML::Key << "Assets";
        out << YAML::Value << YAML::BeginSeq;
        auto& registryMap = registry.at(m_ProjectRegistryID);
        for(auto& [handle, metadata] : registryMap)
        {
            if (metadata.Location != AssetLocation::FileSystem)
            {
                continue;
            }
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << metadata.Name;
            out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
            out << YAML::Key << "Handle" << YAML::Value << handle;
            auto path = std::get<std::filesystem::path>(metadata.Data);
            if(path.is_absolute())
            {
                path = std::filesystem::relative(path, m_ProjectPath);
            }
            out << YAML::Key << "FilePath" << YAML::Value << ResourceManager::ProcessFilePath(path.string());
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(path, std::ios::out);
        fout << out.c_str();
    }

    void AssetRegistrySerializer::Deserialize(const std::filesystem::path &path)
    {
        YAML::Node data = YAML::LoadFile(path.string());
        UUID registryID = data["Registry ID"].as<uint64_t>();
        for(auto asset : data["Assets"])
        {
            std::filesystem::path filePath = asset["FilePath"].as<std::string>();
            if(filePath.is_relative())
            {
                filePath = m_ProjectPath / filePath;
            }
            if(!std::filesystem::exists(filePath))
            {
                BeeCoreError("Asset file {0} does not exist!", filePath.string());
                continue;
            }
            m_AssetManager->LoadAsset(filePath, {registryID,asset["Handle"].as<uint64_t>()});
        }
    }
}
