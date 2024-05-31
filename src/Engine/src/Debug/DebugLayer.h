//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Core/Layer.h"
#include "Gui/ImGui/RendererStatisticsGUI.h"

namespace BeeEngine
{
    namespace Debug
    {
        class DebugLayer : public Layer
        {
        public:
            DebugLayer(){};
            ~DebugLayer() override{};
            void OnAttach() override;
            void OnDetach() override;
            void OnUpdate(FrameData& frameData) override;
            void OnGUIRendering() override;
            void OnEvent(EventDispatcher& e) override;

        private:
            Internal::RendererStatisticsGUI m_RendererStatisticsGUI;
        };
    } // namespace Debug
} // namespace BeeEngine
