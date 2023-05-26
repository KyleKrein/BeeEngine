//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"
#include "ViewPort.h"

namespace BeeEngine::Editor
{
    class EditorLayer: public Layer
    {
    public:
        ~EditorLayer() noexcept override = default;

        void OnAttach() noexcept override;
        void OnDetach() noexcept override;
        void OnUpdate() noexcept override;
        void OnGUIRendering() noexcept override;
        void OnEvent(EventDispatcher& event) noexcept override;
    private:
        void DrawToScene();
        ViewPort m_ViewPort {100, 100, [this]() { DrawToScene(); }};
    };
}
