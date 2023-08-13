//
// Created by alexl on 20.07.2023.
//

#include "ProjectFile.h"
#include "Utils/File.h"
#include "Core/Logging/Log.h"
#include "VSProjectGeneration.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
namespace BeeEngine::Editor
{

    ProjectFile::ProjectFile(const std::filesystem::path &projectPath, const std::string &projectName) noexcept
    : m_ProjectName(projectName), m_ProjectPath(projectPath)
    {
        BeeCoreTrace("ProjectName: {0}", m_ProjectName);
        BeeCoreTrace("ProjectPath: {0}", m_ProjectPath.string());
        if(!std::filesystem::exists(m_ProjectPath / ".beeengine"))
        {
            std::filesystem::create_directory(m_ProjectPath / ".beeengine");
        }
        if(!std::filesystem::exists(m_ProjectPath / ".beeengine" /"build"))
        {
            std::filesystem::create_directory(m_ProjectPath / ".beeengine" / "build");
        }
        m_AppAssemblyPath = m_ProjectPath / ".beeengine" / "build";
        if(File::Exists(m_AppAssemblyPath))
            m_AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>((m_AppAssemblyPath).string(), [this](const std::string & path, filewatch::Event event) { OnAppAssemblyFileSystemEvent(path, event); });
        m_AppAssemblyPath = m_AppAssemblyPath / "GameLibrary.dll";
        std::filesystem::copy_file(std::filesystem::current_path() / "libs" / "BeeEngine.Core.dll", m_ProjectPath / ".beeengine" / "BeeEngine.Core.dll", std::filesystem::copy_options::overwrite_existing);
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

            VSProjectGeneration::GenerateAssemblyInfoFile(m_ProjectPath, m_ProjectName);
            RegenerateSolution();
            return;
        }
        else
        {
            RegenerateSolution();
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
        auto p = path;
        if(path.is_absolute())
            p = std::filesystem::relative(path, m_ProjectPath);
        if(!File::Exists(m_ProjectPath / p))
        {
            BeeCoreWarn("Scene file {0} does not exist!", p.string());
            m_LastUsedScenePath = "";
            Save();
            return;
        }
        m_LastUsedScenePath = p;
        Save();
    }

    void ProjectFile::RegenerateSolution()
    {
        auto sources = VSProjectGeneration::GetSourceFiles(m_ProjectPath);
        VSProjectGeneration::GenerateProject(m_ProjectPath, sources, m_ProjectName);
    }

    void ProjectFile::OnAppAssemblyFileSystemEvent(const std::string &path, const filewatch::Event changeType)
    {
        if(!m_AssemblyReloadPending && (changeType == filewatch::Event::modified || changeType == filewatch::Event::added ))
        {
            m_AssemblyReloadPending = true;
        }
    }

    void ProjectFile::Update() noexcept
    {
        if(m_AppAssemblyFileWatcher)
            return;
        if(File::Exists(m_AppAssemblyPath))
        {
            m_AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>((m_AppAssemblyPath).string(), [this](const std::string & path, filewatch::Event event) { OnAppAssemblyFileSystemEvent(path, event); });
        }
    }
}
