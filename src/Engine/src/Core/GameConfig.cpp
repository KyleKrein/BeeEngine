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
        config.Name = String{data["Name"].as<std::string>()};
        config.StartingScene = String{data["StartingScene"].as<std::string>()};
        config.DefaultLocale = Locale::Localization(String{data["DefaultLocale"].as<std::string>()});
        return config;
    }
    void GameConfig::Serialize(const Path& path) const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << Name.c_str();
        out << YAML::Key << "StartingScene" << YAML::Value << StartingScene.AsUTF8().c_str();
        out << YAML::Key << "DefaultLocale" << YAML::Value << DefaultLocale.GetLanguageString().c_str();
        out << YAML::EndMap;
        File::WriteFile(path, String{out.c_str()});
    }
} // namespace BeeEngine