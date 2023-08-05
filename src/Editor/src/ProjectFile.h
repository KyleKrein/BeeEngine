//
// Created by alexl on 20.07.2023.
//

#pragma once
#include <filesystem>
#include "FileWatch.hpp"
#include "Core/TypeDefines.h"

namespace BeeEngine::Editor
{
    class ProjectFile
    {
    public:
        ProjectFile(const std::filesystem::path& projectPath, const std::string& projectName) noexcept;

        [[nodiscard]] const std::filesystem::path& GetProjectPath() const noexcept;
        [[nodiscard]] const std::string& GetProjectName() const noexcept;
        [[nodiscard]] const std::filesystem::path& GetProjectFilePath() const noexcept;

        std::filesystem::path GetLastUsedScenePath() const noexcept
        {
            return m_LastUsedScenePath.empty() ? m_LastUsedScenePath : m_ProjectPath / m_LastUsedScenePath;
        }
        void SetLastUsedScenePath(const std::filesystem::path& path) noexcept;

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

        void RenameProject(const std::string& newName) noexcept;

        void RegenerateSolution();

        void Save() noexcept;

        void Update() noexcept;

    private:
        void OnAppAssemblyFileSystemEvent(const std::string& path, filewatch::Event changeType);

        std::filesystem::path m_ProjectPath;
        std::string m_ProjectName;
        const std::filesystem::path m_ProjectFilePath = m_ProjectPath / (m_ProjectName + ".beeproj");
        std::filesystem::path m_LastUsedScenePath {""};
        Scope<filewatch::FileWatch<std::string>> m_AppAssemblyFileWatcher = nullptr;
        mutable bool m_AssemblyReloadPending = false;
        std::filesystem::path m_AppAssemblyPath;
    };
}
