//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"
#include "Gui/ImGui/IImGuiElement.h"

namespace BeeEngine::Editor
{
    class ViewPort final: public Internal::IImGuiElement
    {
    public:
        ViewPort(uint32_t width, uint32_t height, const std::function<void()>& renderFunction) noexcept;
        void OnEvent(EventDispatcher& event) noexcept override;
        void Update() noexcept override;
        void Render() noexcept override;
    private:
        uint32_t m_Width;
        uint32_t m_Height;
        Scope<FrameBuffer> m_FrameBuffer;
        bool m_IsFocused;
        bool m_IsHovered;
        OrthographicCameraController m_CameraController;
        std::function<void()> m_RenderFunction;
    };
}
