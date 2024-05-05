//
// Created by alexl on 16.08.2023.
//

#include "AssetRegistrySerializer.h"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

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
            case AssetType::Prefab:
                return "AssetType::Prefab";
            case AssetType::Scene:
                return "AssetType::Scene";
                /*case AssetType::Shader:
                    return "AssetType::Shader";
                case AssetType::Mesh:
                    return "AssetType::Mesh";
                case AssetType::Material:
                    return "AssetType::Material";
                case AssetType::Model:
                    return "AssetType::Model";*/
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
        if (type == "AssetType::Prefab")
            return AssetType::Prefab;
        if (type == "AssetType::Scene")
            return AssetType::Scene;
        /*if (type == "AssetType::Shader")
            return AssetType::Shader;
        if (type == "AssetType::Mesh")
            return AssetType::Mesh;
        if (type == "AssetType::Material")
            return AssetType::Material;
        if (type == "AssetType::Model")
            return AssetType::Model;*/
        BeeCoreError("Unknown AssetType: {0}", type);
        return AssetType::None;
    }

    void AssetRegistrySerializer::Serialize(const Path& path)
    {
        auto& registry = m_AssetManager->GetAssetRegistry();
        if (!registry.contains(m_ProjectRegistryID))
            return;
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Registry ID" << YAML::Value << m_ProjectRegistryID;
        out << YAML::Key << "Assets";
        out << YAML::Value << YAML::BeginSeq;
        auto& registryMap = registry.at(m_ProjectRegistryID);
        for (auto& [handle, metadata] : registryMap)
        {
            if (metadata.Location != AssetLocation::FileSystem)
            {
                continue;
            }
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << metadata.Name;
            out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
            out << YAML::Key << "Handle" << YAML::Value << handle;
            auto filepath = std::get<Path>(metadata.Data);
            if (filepath.IsAbsolute())
            {
                filepath = filepath.GetRelativePath(m_ProjectPath);
            }
            out << YAML::Key << "FilePath" << YAML::Value << filepath.AsUTF8();
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        File::WriteFile(path, out.c_str());
    }

    void AssetRegistrySerializer::Deserialize(const Path& path)
    {
        std::ifstream ifs(path.ToStdPath());
        YAML::Node data = YAML::Load(ifs);
        ifs.close();
        UUID registryID = data["Registry ID"].as<uint64_t>();
        for (auto asset : data["Assets"])
        {
            Path filePath = asset["FilePath"].as<std::string>();
            if (filePath.IsRelative())
            {
                filePath = m_ProjectPath / filePath;
            }
            if (!File::Exists(filePath))
            {
                BeeCoreError("Asset file {0} does not exist!", filePath.AsUTF8());
                continue;
            }
            m_AssetManager->LoadAsset(filePath, {registryID, asset["Handle"].as<uint64_t>()});
        }
    }
} // namespace BeeEngine
