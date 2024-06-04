//
// Created by alexl on 26.05.2023.
//

#include "ConfigFile.h"
#include "Core/Move.h"
#include "FileSystem/File.h"
#include "Locale/Locale.h"
#include "Serialization/YAMLHelper.h"
#include "Windowing/VSync.h"

namespace BeeEngine::Editor
{
    void ConfigFile::Save(const Path& path) const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Width" << YAML::Value << Width;
        out << YAML::Key << "Height" << YAML::Value << Height;
        out << YAML::Key << "X" << YAML::Value << X;
        out << YAML::Key << "Y" << YAML::Value << Y;
        out << YAML::Key << "Locale" << YAML::Value << Locale.GetLanguageString().c_str();
        out << YAML::Key << "FontSize" << YAML::Value << FontSize;
        out << YAML::Key << "ThumbnailSize" << YAML::Value << ThumbnailSize;
        out << YAML::Key << "VSYNC" << YAML::Value << ToString(VSYNC).c_str();
        out << YAML::Key << "IsMaximized" << YAML::Value << IsMaximized;
        out << YAML::EndMap;
        File::WriteFile(path, String(out.c_str()));
    }
    ConfigFile ConfigFile::Load(const Path& path)
    {
        ConfigFile config;
        if (!File::Exists(path))
        {
            config.Save(path);
            return config;
        }
        auto data = File::ReadFile(path);
        YAML::Node node = YAML::Load(data.c_str());
        if (node["Width"])
        {
            config.Width = node["Width"].as<uint16_t>();
        }
        if (node["Height"])
        {
            config.Height = node["Height"].as<uint16_t>();
        }
        if (node["X"])
        {
            config.X = node["X"].as<uint16_t>();
        }
        if (node["Y"])
        {
            config.Y = node["Y"].as<uint16_t>();
        }
        if (node["Locale"])
        {
            Locale::Localization locale{node["Locale"].as<std::string>().c_str()};
            config.Locale = BeeMove(locale);
        }
        if (node["FontSize"])
        {
            config.FontSize = node["FontSize"].as<float>();
        }
        if (node["ThumbnailSize"])
        {
            config.ThumbnailSize = node["ThumbnailSize"].as<float>();
        }
        if (node["VSYNC"])
        {
            config.VSYNC = StringToEnum<VSync>(node["VSYNC"].as<std::string>().c_str());
        }
        if (node["IsMaximized"])
        {
            config.IsMaximized = node["IsMaximized"].as<bool>();
        }
        return config;
    }
    ApplicationProperties ConfigFile::GetApplicationProperties() noexcept
    {
        return {.WindowWidth = Width,
                .WindowHeight = Height,
                .Title = "BeeEngine Editor",
                .Vsync = VSYNC,
                .WindowXPosition = X,
                .WindowYPosition = Y,
                .IsMaximized = IsMaximized,
                .Mode = ApplicationProperties::StartMode::UI};
    }

    std::filesystem::path ConfigFile::LoadCompilerConfiguration() noexcept
    {
        return std::filesystem::current_path();
    }
} // namespace BeeEngine::Editor
