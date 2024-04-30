//
// Created by alexl on 20.07.2023.
//

#pragma once
#include <filesystem>
#include "Core/TypeDefines.h"
#include "Core/UUID.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "FileSystem/FileWatcher.h"
#include "Locale/Locale.h"
#include "Core/OsPlatform.h"

namespace BeeEngine::Editor
{
    struct BuildProjectOptions
    {
        enum class BuildType
        {
            Debug,
            Release
        };
        BuildType BuildType = BuildType::Debug;
        Path OutputPath;
        Locale::Localization DefaultLocale = Locale::Localization::Default;
    };
    class ProjectFile
    {
    public:
        ProjectFile(const Path& projectPath, const std::string& projectName, EditorAssetManager* assetManager) noexcept;

        [[nodiscard]] const Path& GetProjectPath() const noexcept;
        [[nodiscard]] const std::string& GetProjectName() const noexcept;
        [[nodiscard]] const Path& GetProjectFilePath() const noexcept;
        [[nodiscard]] const Path& GetProjectAssetRegistryPath() const noexcept;

        Path GetLastUsedScenePath() const noexcept
        {
            return m_LastUsedScenePath.IsEmpty() ? m_LastUsedScenePath : m_ProjectPath / m_LastUsedScenePath;
        }
        void SetLastUsedScenePath(const Path& path) noexcept;

        void SetLastUsedSceneToNull() noexcept
        {
            m_LastUsedScenePath = "";
            Save();
        }

        bool IsAssemblyReloadPending() const noexcept
        {
            bool temp = m_AssemblyReloadPending;
            m_AssemblyReloadPending = false;
            return temp;
        }

        UUID GetAssetRegistryID() const noexcept
        {
            return m_AssetRegistryID;
        }

        void RenameProject(const std::string& newName) noexcept;

        void RegenerateSolution();

        void BuildProject(const BuildProjectOptions& options);

        void Save() noexcept;

        void Update() noexcept;

        Locale::Domain& GetProjectLocaleDomain() noexcept
        {
            return m_ProjectLocaleDomain;
        }

        const Path& GetAssemblyPath() const
        {
            return m_AppAssemblyPath;
        }

        void ReloadAndRebuildGameLibrary();

    private:
        std::vector<std::pair<OSPlatform, Path>> CheckForAvailablePlatforms();
        Path BuildWindowsGame(const Path& gameLibraryPath, const Path& outputDirectory);
        Path BuildMacOSGame(const Path& gameLibraryPath, const Path& outputDirectory);
        void OnAppAssemblyFileSystemEvent(const Path& path, FileWatcher::Event changeType);
        void OnAssetFileSystemEvent(const Path& path, FileWatcher::Event changeType);

        void LoadLocalizationFiles();

        Path m_ProjectPath;
        std::string m_ProjectName;
        const Path m_ProjectFilePath = m_ProjectPath / (m_ProjectName + ".beeproj");
        const Path m_ProjectAssetRegistryPath = m_ProjectPath / (m_ProjectName + ".beeassetregistry");
        Path m_LastUsedScenePath {""};
        Scope<FileWatcher> m_AppAssemblyFileWatcher = nullptr;
        mutable bool m_AssemblyReloadPending = false;
        Path m_AppAssemblyPath;
        BeeEngine::UUID m_AssetRegistryID;
        Locale::Domain m_ProjectLocaleDomain;

        Scope<FileWatcher> m_AssetFileWatcher = nullptr;
        EditorAssetManager* m_AssetManager;

        void HandleChangedScriptFile(const Path &path, FileWatcher::Event event);

        bool m_MustReload;
    };
}
