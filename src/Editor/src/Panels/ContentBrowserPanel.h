//
// Created by alexl on 07.06.2023.
//

#pragma once
#include <filesystem>
#include "Renderer/Texture.h"

namespace BeeEngine::Editor
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel(const std::filesystem::path& workingDirectory) noexcept;
        void SetWorkingDirectory(const std::filesystem::path& path) noexcept
        {
            m_WorkingDirectory = path;
            m_CurrentDirectory = path;
        }
        [[nodiscard]] std::filesystem::path GetWorkingDirectory() const noexcept
        {
            return m_WorkingDirectory;
        }

        bool NeedsToRegenerateSolution() noexcept
        {
            bool temp = m_NeedToRegenerateSolution;
            m_NeedToRegenerateSolution = false;
            return temp;
        }

        void OnGUIRender() noexcept;
    private:
        std::filesystem::path m_WorkingDirectory;
        std::filesystem::path m_CurrentDirectory;

        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
        bool m_NeedToRegenerateSolution = false;

        void DragAndDropFileToFolder(const std::filesystem::path &path);
    };
}
