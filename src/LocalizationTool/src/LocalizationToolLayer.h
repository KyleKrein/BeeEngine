//
// Created by alexl on 19.10.2023.
//

#pragma once

#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/Layer.h"
#include "Locale/ImGuiLocalizationPanel.h"
#include "Locale/Locale.h"

namespace BeeEngine::LocalizationTool
{
    class LocalizationToolLayer : public Layer
    {
    public:
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(FrameData& frameData) override;
        void OnGUIRendering() override;
        void OnEvent(BeeEngine::EventDispatcher& e) override;

    private:
        enum class State
        {
            FolderChoice,
            Waiting,
            Editor
        } m_State;
        Jobs::SpinLock m_Lock;
        EditorAssetManager m_AssetManager;
        Ref<Locale::ImGuiLocalizationPanel> m_LocalizationPanel;
        Locale::Domain m_Domain{"Localization"};
    };
} // namespace BeeEngine::LocalizationTool
