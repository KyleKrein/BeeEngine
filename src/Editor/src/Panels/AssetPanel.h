//
// Created by alexl on 19.08.2023.
//

#pragma once

#include "ProjectFile.h"
#include "Core/AssetManagement/EditorAssetManager.h"

namespace BeeEngine::Editor
{
    class AssetPanel
    {
    public:
        AssetPanel(EditorAssetManager* assetManager);
        void SetProject(ProjectFile* project);

        void Render();
    private:
        EditorAssetManager* m_AssetManager;
        ProjectFile* m_Project;

        const char *GetDragAndDropTypeName(AssetType type);
    };
}
