//
// Created by alexl on 20.07.2023.
//

#pragma once
#include <filesystem>

namespace BeeEngine::Editor
{
    class ProjectFile
    {
    public:
        ProjectFile(const std::filesystem::path& projectPath, const std::string& projectName) noexcept;

        [[nodiscard]] const std::filesystem::path& GetProjectPath() const noexcept;
        [[nodiscard]] const std::string& GetProjectName() const noexcept;
        [[nodiscard]] const std::filesystem::path& GetProjectFilePath() const noexcept;

        const std::filesystem::path& GetLastUsedScenePath() const noexcept
        {
            return m_LastUsedScenePath;
        }
        void SetLastUsedScenePath(const std::filesystem::path& path) noexcept;

        void SetLastUsedSceneToNull() noexcept
        {
            m_LastUsedScenePath = "";
            Save();
        }

        void RenameProject(const std::string& newName) noexcept;

        void Save() noexcept;
    private:
        std::filesystem::path m_ProjectPath;
        std::string m_ProjectName;
        const std::filesystem::path m_ProjectFilePath = m_ProjectPath / (m_ProjectName + ".beeproj");
        std::filesystem::path m_LastUsedScenePath {""};
    };
}
