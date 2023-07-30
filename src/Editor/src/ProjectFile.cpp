//
// Created by alexl on 20.07.2023.
//

#include "ProjectFile.h"
#include "Utils/File.h"
#include "Core/Logging/Log.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
namespace BeeEngine::Editor
{

    ProjectFile::ProjectFile(const std::filesystem::path &projectPath, const std::string &projectName) noexcept
    : m_ProjectName(projectName), m_ProjectPath(projectPath)
    {
        BeeCoreTrace("ProjectName: {0}", m_ProjectName);
        BeeCoreTrace("ProjectPath: {0}", m_ProjectPath.string());
        if(!File::Exists(m_ProjectFilePath))
        {
            init:
            Save();
            auto currentConfigFile = std::filesystem::current_path() / "projects.cfg";
            if(!File::Exists(currentConfigFile))
            {
                std::ofstream fout(currentConfigFile, std::ios::out);
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "Projects";
                out << YAML::BeginSeq;
                out << YAML::BeginMap;
                out << YAML::Key << "ProjectName" << YAML::Value << m_ProjectName;
                out << YAML::Key << "ProjectPath" << YAML::Value << m_ProjectPath.string();
                out << YAML::EndMap;
                out << YAML::EndSeq;
                out << YAML::EndMap;
                fout << "LastProject: " << m_ProjectName;
                fout.close();
            }
            else
            {
                /*YAML::Node data = YAML::LoadFile(currentConfigFile.string());
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "ProjectName" << YAML::Value << m_ProjectName;
                out << YAML::Key << "ProjectPath" << YAML::Value << m_ProjectPath.string();
                out << YAML::EndMap;
                data["Projects"].push_back(YAML::Load(out.c_str()));
                std::ofstream fout(currentConfigFile);
                fout << data;*/
            }
            std::filesystem::create_directory(m_ProjectPath / "Assets");
            std::filesystem::create_directory(m_ProjectPath / "Scenes");
            return;
        }
        YAML::Node data = YAML::LoadFile(m_ProjectFilePath.string());
        if(!data["ProjectName"])
        {
            goto init;
        }
        SetLastUsedScenePath(data["LastUsedScene"].as<std::string>());
    }

    const std::filesystem::path &ProjectFile::GetProjectPath() const noexcept
    {
        return m_ProjectPath;
    }

    const std::string &ProjectFile::GetProjectName() const noexcept
    {
        return m_ProjectName;
    }

    void ProjectFile::RenameProject(const std::string &newName) noexcept
    {

    }

    void ProjectFile::Save() noexcept
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "ProjectName" << YAML::Value << m_ProjectName;
        out << YAML::Key << "LastUsedScene" << YAML::Value << m_LastUsedScenePath.string();
        out << YAML::EndMap;

        std::ofstream fout(m_ProjectFilePath, std::ios::out);
        fout << out.c_str();
        fout.close();
    }

    void ProjectFile::SetLastUsedScenePath(const std::filesystem::path &path) noexcept
    {
        if(!File::Exists(path))
        {
            BeeCoreWarn("Scene file {0} does not exist!", path.string());
            m_LastUsedScenePath = "";
            Save();
            return;
        }
        if(m_LastUsedScenePath.is_absolute())
            m_LastUsedScenePath = std::filesystem::relative(m_LastUsedScenePath, m_ProjectPath);
        else
            m_LastUsedScenePath = path;
        Save();
    }
}
