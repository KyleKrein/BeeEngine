#pragma once
#include "Core/AssetManagement/EditorAssetManager.h"
#include "ProjectFile.h"
namespace BeeEngine::Editor
{
    class ProjectSettings
    {
    public:
        ProjectSettings(ProjectFile& currentProject, Locale::Domain& domain, EditorAssetManager& assetManager);
        void Toggle() { m_IsOpened = !m_IsOpened; }
        void Render();

    private:
        ProjectFile& m_CurrentProject;
        Locale::Domain& m_Domain;
        BeeEngine::EditorAssetManager& m_AssetManager;
        String m_ProjectName;
        AssetHandle m_IconHandle;
        bool m_IsOpened = false;

        void RenderIconsSelector();
    };
} // namespace BeeEngine::Editor