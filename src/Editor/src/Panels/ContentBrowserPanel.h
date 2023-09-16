//
// Created by alexl on 07.06.2023.
//

#pragma once
#include <filesystem>
#include "Renderer/Texture.h"
#include "Core/Path.h"

namespace BeeEngine::Editor
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel(const Path& workingDirectory) noexcept;
        void SetWorkingDirectory(const Path& path) noexcept
        {
            m_WorkingDirectory = path;
            m_CurrentDirectory = path;
        }
        [[nodiscard]] Path GetWorkingDirectory() const noexcept
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
        Path m_WorkingDirectory;
        Path m_CurrentDirectory;

        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
        bool m_NeedToRegenerateSolution = false;

        void DragAndDropFileToFolder(const Path &path);
    };
}
