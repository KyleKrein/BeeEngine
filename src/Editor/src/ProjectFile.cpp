//
// Created by alexl on 20.07.2023.
//

#include "ProjectFile.h"
#include "Utils/File.h"
#include "Core/Logging/Log.h"
#include "VSProjectGeneration.h"
#include "Core/ResourceManager.h"
#include "Core/AssetManagement/AssetRegistrySerializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
namespace BeeEngine::Editor
{

    ProjectFile::ProjectFile(const std::filesystem::path &projectPath, const std::string &projectName, EditorAssetManager* assetManager) noexcept
    : m_ProjectName(projectName), m_ProjectPath(projectPath), m_AssetManager(assetManager)
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
            m_AssetFileWatcher = CreateScope<filewatch::FileWatch<std::string>>((m_ProjectPath).string(), [this](const std::string & path, filewatch::Event event) { OnAssetFileSystemEvent(path, event); });
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
        m_AssetRegistryID = data["Asset Registry ID"].as<uint64_t>();
        SetLastUsedScenePath(data["LastUsedScene"].as<std::string>());
        m_AssetFileWatcher = CreateScope<filewatch::FileWatch<std::string>>((m_ProjectPath).string(), [this](const std::string & path, filewatch::Event event) { OnAssetFileSystemEvent(path, event); });
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
        out << YAML::Key << "Asset Registry ID" << YAML::Value << (uint64_t)m_AssetRegistryID;
        out << YAML::Key << "LastUsedScene" << YAML::Value << ResourceManager::ProcessFilePath(m_LastUsedScenePath.string());
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

    const std::filesystem::path &ProjectFile::GetProjectFilePath() const noexcept
    {
        return m_ProjectFilePath;
    }

    const std::filesystem::path &ProjectFile::GetProjectAssetRegistryPath() const noexcept
    {
        return m_ProjectAssetRegistryPath;
    }

    void ProjectFile::OnAssetFileSystemEvent(const std::string &path, filewatch::Event changeType)
    {
        std::filesystem::path p = path;
        if(!ResourceManager::IsAssetExtension(p.extension()))
            return;
        if(p.is_relative())
            p = m_ProjectPath / p;
        std::string name = ResourceManager::GetNameFromFilePath(p.string());
        const AssetHandle* handlePtr = m_AssetManager->GetAssetHandleByName(name);
        if(!handlePtr)
            return;
        AssetHandle handle = *handlePtr;
        bool changed = false;

        switch (changeType)
        {
            case filewatch::Event::added:
                Application::SubmitToMainThread([this, p, handle]()
                {
                    m_AssetManager->LoadAsset(p, handle);
                });
                changed = true;
                break;
            case filewatch::Event::removed:
                break;
            case filewatch::Event::modified:
                if(m_AssetManager->IsAssetLoaded(handle))
                {
                    Application::SubmitToMainThread([this, p, handle]()
                    {
                        if(m_AssetManager->IsAssetLoaded(handle))
                            m_AssetManager->UnloadAsset(handle);
                    });
                }
                break;
            case filewatch::Event::renamed_old:
                break;
            case filewatch::Event::renamed_new:
                break;
        }
        if(changed)
        {
            Application::SubmitToMainThread([this]()
                                            {
                                                AssetRegistrySerializer serializer(m_AssetManager, m_ProjectPath, m_AssetRegistryID);
                                                serializer.Serialize(m_ProjectAssetRegistryPath);
                                            });

        }
    }
}
