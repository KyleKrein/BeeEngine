//
// Created by alexl on 20.07.2023.
//

#include "ProjectFile.h"
#include "FileSystem/File.h"
#include "Core/Logging/Log.h"
#include "VSProjectGeneration.h"
#include "Core/ResourceManager.h"
#include "Core/AssetManagement/AssetRegistrySerializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>
#include "Locale/LocalizationGenerator.h"
namespace BeeEngine::Editor
{

    ProjectFile::ProjectFile(const Path &projectPath, const std::string &projectName, EditorAssetManager* assetManager) noexcept
    : m_ProjectName(projectName), m_ProjectPath(projectPath), m_AssetManager(assetManager), m_ProjectLocaleDomain(projectName)
    {
        BeeCoreTrace("ProjectName: {0}", m_ProjectName);
        BeeCoreTrace("ProjectPath: {0}", m_ProjectPath.AsUTF8());
        LoadLocalizationFiles();
        if(!File::Exists(m_ProjectPath / ".beeengine"))
        {
            File::CreateDirectory(m_ProjectPath / ".beeengine");
        }
        if(!File::Exists(m_ProjectPath / ".beeengine" /"build"))
        {
            File::CreateDirectory(m_ProjectPath / ".beeengine" / "build");
        }
        m_AppAssemblyPath = m_ProjectPath / ".beeengine" / "build";
        if(File::Exists(m_AppAssemblyPath))
            m_AppAssemblyFileWatcher = FileWatcher::Create(m_AppAssemblyPath, [this](const Path & path, FileWatcher::Event event) { OnAppAssemblyFileSystemEvent(path, event); });
        m_AppAssemblyPath = m_AppAssemblyPath / "GameLibrary.dll";
        std::filesystem::copy_file(std::filesystem::current_path() / "libs" / "BeeEngine.Core.dll", m_ProjectPath.ToStdPath() / ".beeengine" / "BeeEngine.Core.dll", std::filesystem::copy_options::overwrite_existing);
        if(!File::Exists(m_ProjectFilePath))
        {
            init:
            Save();
            Path currentConfigFile = std::filesystem::current_path() / "projects.cfg";
            if(!File::Exists(currentConfigFile))
            {
                /*std::ofstream fout(currentConfigFile, std::ios::out);
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "Projects";
                out << YAML::BeginSeq;
                out << YAML::BeginMap;
                out << YAML::Key << "ProjectName" << YAML::Value << m_ProjectName;
                out << YAML::Key << "ProjectPath" << YAML::Value << m_ProjectPath.AsUTF8();
                out << YAML::EndMap;
                out << YAML::EndSeq;
                out << YAML::EndMap;
                fout << "LastProject: " << m_ProjectName;
                fout.close();*/
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
            File::CreateDirectory(m_ProjectPath / "Assets");
            File::CreateDirectory(m_ProjectPath / "Scenes");

            VSProjectGeneration::GenerateAssemblyInfoFile(m_ProjectPath, m_ProjectName);
            RegenerateSolution();
            m_AssetFileWatcher = FileWatcher::Create(m_ProjectPath, [this](const Path & path, FileWatcher::Event event) { OnAssetFileSystemEvent(path, event); });
            return;
        }
        else
        {
            RegenerateSolution();
        }
        std::ifstream ifs(m_ProjectFilePath.ToStdPath());
        YAML::Node data = YAML::Load(ifs);
        ifs.close();
        if(!data["ProjectName"])
        {
            goto init;
        }
        m_AssetRegistryID = data["Asset Registry ID"].as<uint64_t>();
        SetLastUsedScenePath(data["LastUsedScene"].as<std::string>());
        m_AssetFileWatcher = FileWatcher::Create(m_ProjectPath, [this](const Path & path, FileWatcher::Event event) { OnAssetFileSystemEvent(path, event); });
    }

    const Path &ProjectFile::GetProjectPath() const noexcept
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
        out << YAML::Key << "LastUsedScene" << YAML::Value << m_LastUsedScenePath.AsUTF8();
        out << YAML::EndMap;

        File::WriteFile(m_ProjectFilePath, out.c_str());
    }

    void ProjectFile::SetLastUsedScenePath(const Path &path) noexcept
    {
        auto p = path;
        if(path.IsAbsolute())
            p = path.GetRelativePath(m_ProjectPath);
        if(!File::Exists(m_ProjectPath / p))
        {
            BeeCoreWarn("Scene file {0} does not exist!", p.AsUTF8());
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

    void ProjectFile::OnAppAssemblyFileSystemEvent(const Path &path, const FileWatcher::Event changeType)
    {
        if(!m_AssemblyReloadPending && (changeType == FileWatcher::Event::Modified || changeType == FileWatcher::Event::Added ))
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
            m_AppAssemblyFileWatcher = FileWatcher::Create(m_AppAssemblyPath, [this](const Path & path, FileWatcher::Event event) { OnAppAssemblyFileSystemEvent(path, event); });
        }
    }

    const Path &ProjectFile::GetProjectFilePath() const noexcept
    {
        return m_ProjectFilePath;
    }

    const Path &ProjectFile::GetProjectAssetRegistryPath() const noexcept
    {
        return m_ProjectAssetRegistryPath;
    }

    void ProjectFile::OnAssetFileSystemEvent(const Path &path, FileWatcher::Event changeType)
    {
        Path p = path;
        if(!ResourceManager::IsAssetExtension(p.GetExtension()))
            return;
        if(p.IsRelative())
            p = m_ProjectPath / p;
        std::string name = p.GetFileNameWithoutExtension();
        const AssetHandle* handlePtr = m_AssetManager->GetAssetHandleByName(name);
        if(!handlePtr and changeType != FileWatcher::Event::RenamedNewName)
        {
            return;
        }
        static Path oldNameOnRenamed;
        if(changeType == FileWatcher::Event::RenamedNewName)
        {
            BeeCoreTrace("Renamed {0} to {1}", oldNameOnRenamed.AsUTF8(), p.AsUTF8());
            Application::SubmitToMainThread([oldPath = p, newPath = oldNameOnRenamed]()
                                            {
                                                //m_AssetManager->RenameAsset(oldPath, newPath, handle);
                                            });
            return;
        }
        AssetHandle handle = *handlePtr;
        bool changed = false;

        switch (changeType)
        {
            case FileWatcher::Event::Added:
                Application::SubmitToMainThread([this, p, handle]()
                {
                    m_AssetManager->LoadAsset(p, handle);
                });
                changed = true;
                break;
            case FileWatcher::Event::Removed:
                break;
            case FileWatcher::Event::Modified:
                if(m_AssetManager->IsAssetLoaded(handle))
                {
                    Application::SubmitToMainThread([this, p, handle]()
                    {
                        if(m_AssetManager->IsAssetLoaded(handle))
                            m_AssetManager->UnloadAsset(handle);
                    });
                }
                break;
            case FileWatcher::Event::RenamedOldName:
                oldNameOnRenamed = p;
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

    void ProjectFile::LoadLocalizationFiles()
    {
        auto paths = Locale::LocalizationGenerator::GetLocalizationFiles(m_ProjectPath);
        Locale::LocalizationGenerator::ProcessLocalizationFiles(m_ProjectLocaleDomain, paths);
        m_ProjectLocaleDomain.Build();
    }
}
