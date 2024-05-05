#include "GameConfig.h"
#include "FileSystem/File.h"
#include <yaml-cpp/yaml.h>

namespace BeeEngine
{
    GameConfig GameConfig::Deserialize(const Path& path)
    {
        GameConfig config;
        String str = File::ReadFile(path);
        YAML::Node data = YAML::Load(str.c_str());
        config.Name = data["Name"].as<String>();
        config.StartingScene = data["StartingScene"].as<String>();
        config.DefaultLocale = Locale::Localization(data["DefaultLocale"].as<String>());
        return config;
    }
    void GameConfig::Serialize(const Path& path) const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << Name;
        out << YAML::Key << "StartingScene" << YAML::Value << StartingScene.AsUTF8();
        out << YAML::Key << "DefaultLocale" << YAML::Value << DefaultLocale.GetLanguageString();
        out << YAML::EndMap;
        File::WriteFile(path, out.c_str());
    }
} // namespace BeeEngine