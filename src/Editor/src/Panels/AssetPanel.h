//
// Created by alexl on 19.08.2023.
//

#pragma once

#include "AssetEditPanel.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/Coroutines/Generator.h"
#include "Core/String.h"
#include "Locale/Locale.h"
#include "ProjectFile.h"
#include <string_view>

namespace BeeEngine::Editor
{
    class AssetPanel
    {
    public:
        AssetPanel(EditorAssetManager* assetManager, Locale::Domain& domain);
        void SetProject(ProjectFile* project);

        void SetAssetDeletedCallback(const std::function<void(AssetHandle)>& callback) { m_OnAssetDeleted = callback; }

        void Render();

        static const char* GetDragAndDropTypeName(AssetType type);

    private:
        struct Filter
        {
            AssetPanel& self;
            AssetType Type = AssetType::None;
            String Name;
            String NameLower;
            bool FilterByName(std::string_view name) const
            {
                return Name.empty() || ToLowercase(name).find(NameLower) != String::npos;
            }
            Generator<Asset&> FilterAssets() const
            {
                for (auto& asset : self.m_AssetManager->IterateAssets())
                {
                    if (Type != AssetType::None && asset.GetType() != Type)
                    {
                        continue;
                    }
                    if (!FilterByName(asset.Name))
                    {
                        continue;
                    }
                    co_yield asset;
                }
            }
        } m_Filter{*this};
        friend Filter;
        Scope<AssetEditPanel> m_AssetEditPanel;
        EditorAssetManager* m_AssetManager;
        Locale::Domain* m_EditorDomain;
        ProjectFile* m_Project;
        std::function<void(AssetHandle)> m_OnAssetDeleted;
    };
} // namespace BeeEngine::Editor
