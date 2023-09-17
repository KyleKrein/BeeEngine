//
// Created by alexl on 20.07.2023.
//

#pragma once
#include <filesystem>
#include "FileWatch.hpp"
#include "Core/TypeDefines.h"
#include "Core/UUID.h"
#include "Core/AssetManagement/EditorAssetManager.h"

namespace BeeEngine::Editor
{
    class ProjectFile
    {
#if defined(WINDOWS)
    using FileWatchString = std::wstring;
#else
    using FileWatchString = std::string;
#endif
        using FileWatcher = filewatch::FileWatch<FileWatchString>;
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

        void Save() noexcept;

        void Update() noexcept;

    private:
        void OnAppAssemblyFileSystemEvent(const FileWatchString& path, filewatch::Event changeType);
        void OnAssetFileSystemEvent(const FileWatchString& path, filewatch::Event changeType);

        Path m_ProjectPath;
        std::string m_ProjectName;
        const Path m_ProjectFilePath = m_ProjectPath / (m_ProjectName + ".beeproj");
        const Path m_ProjectAssetRegistryPath = m_ProjectPath / (m_ProjectName + ".beeassetregistry");
        Path m_LastUsedScenePath {""};
        Scope<FileWatcher> m_AppAssemblyFileWatcher = nullptr;
        mutable bool m_AssemblyReloadPending = false;
        Path m_AppAssemblyPath;
        BeeEngine::UUID m_AssetRegistryID;

        Scope<FileWatcher> m_AssetFileWatcher = nullptr;
        EditorAssetManager* m_AssetManager;
    };
}
