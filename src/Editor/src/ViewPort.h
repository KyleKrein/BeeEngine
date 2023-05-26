//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"

namespace BeeEngine::Editor
{
    class ViewPort final
    {
    public:
        ViewPort(uint32_t width, uint32_t height, const std::function<void()>& renderFunction) noexcept;
        void OnEvent(EventDispatcher& event) noexcept;
        void OnUpdate() noexcept;
        void Render() noexcept;
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
