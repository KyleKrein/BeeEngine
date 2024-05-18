#pragma once
#include "ProjectFile.h"
namespace BeeEngine::Editor
{
    class ProjectSettings
    {
    public:
        ProjectSettings(ProjectFile& currentProject, Locale::Domain& domain);
        void Toggle() { m_IsOpened = !m_IsOpened; }
        void Render();

    private:
        ProjectFile& m_CurrentProject;
        Locale::Domain& m_Domain;
        String m_ProjectName;
        AssetHandle m_IconHandle;
        bool m_IsOpened = false;

        void RenderIconsSelector();
    };
} // namespace BeeEngine::Editor