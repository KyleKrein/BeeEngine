//
// Created by alexl on 07.06.2023.
//

#pragma once
#include "../ProjectFile.h"
#include "ConfigFile.h"
#include "Core/Path.h"
#include "Renderer/Texture.h"
#include "Scene/Scene.h"

namespace BeeEngine::Editor
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel(const Path& workingDirectory,
                            Locale::Domain& editorDomain,
                            const ConfigFile& config) noexcept;
        void SetContext(const Ref<Scene>& context) noexcept { m_Context = context; }
        void SetProject(ProjectFile* project) noexcept { m_Project = project; }
        void SetWorkingDirectory(const Path& path) noexcept
        {
            m_WorkingDirectory = path;
            m_CurrentDirectory = path;
        }
        [[nodiscard]] Path GetWorkingDirectory() const noexcept { return m_WorkingDirectory; }

        bool NeedsToRegenerateSolution() noexcept
        {
            bool temp = m_NeedToRegenerateSolution;
            m_NeedToRegenerateSolution = false;
            return temp;
        }

        void OnGUIRender() noexcept;

    private:
        const ConfigFile& m_Config;
        Path m_WorkingDirectory;
        Path m_CurrentDirectory;
        Ref<Scene> m_Context;
        ProjectFile* m_Project = nullptr;
        Locale::Domain* m_EditorDomain = nullptr;

        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
        bool m_NeedToRegenerateSolution = false;

        void DragAndDropFileToFolder(const Path& path);

        void AcceptExternFilesAndCopy(const Path& folder) const;
    };
} // namespace BeeEngine::Editor
