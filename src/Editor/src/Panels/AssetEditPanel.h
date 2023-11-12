//
// Created by alexl on 11.11.2023.
//

#pragma once
#include "Locale/Locale.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/Application.h"

namespace BeeEngine::Editor
{
    class AssetEditPanel final
    {
    public:
        AssetEditPanel(Locale::Domain& domain, const AssetHandle& handle);

        void Render();
        ~AssetEditPanel();
        void ToggleOpened() noexcept
        {
            m_IsOpened = !m_IsOpened;
        }
        bool IsOpened() const noexcept
        {
            return m_IsOpened;
        }
    private:
        AssetHandle m_Handle;
        Locale::Domain* m_Domain;
        AssetType m_AssetType;
        String m_AssetName;
        bool m_NeedsToBeUnloaded = true;
        bool m_IsOpened = true;

        void RenderLocalizedAssetSettings();

        void HandleLocalizedAssetSave();
    };

} // BeeEngine
