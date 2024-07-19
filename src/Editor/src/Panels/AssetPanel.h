//
// Created by alexl on 19.08.2023.
//

#pragma once

#include "AssetEditPanel.h"
#include "ConfigFile.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/AssetManagement/AssetMetadata.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/Coroutines/Generator.h"
#include "Core/Property.h"
#include "Core/String.h"
#include "Locale/Locale.h"
#include "ProjectFile.h"
#include <string_view>
#include <utility>

namespace BeeEngine::Editor
{
    class AssetPanel
    {
    public:
        AssetPanel(Property<Scope<ProjectFile>>& project,
                   EditorAssetManager* assetManager,
                   Locale::Domain& domain,
                   const ConfigFile& config);

        Signal<AssetHandle> onAssetRemoved;

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
            Generator<std::pair<AssetHandle, const AssetMetadata*>> FilterAssets() const
            {
                for (const auto& [handle, metadata] : self.m_AssetManager->IterateAssetsData())
                {
                    if (Type != AssetType::None && metadata->Type != Type)
                    {
                        continue;
                    }
                    if (!FilterByName(std::string_view(metadata->Name)))
                    {
                        continue;
                    }
                    co_yield std::pair(handle, metadata);
                }
            }
        } m_Filter{*this};
        friend Filter;
        const ConfigFile& m_Config;
        Scope<AssetEditPanel> m_AssetEditPanel;
        EditorAssetManager* m_AssetManager;
        Locale::Domain* m_EditorDomain;
        const ProjectFile* m_Project;
    };
} // namespace BeeEngine::Editor
