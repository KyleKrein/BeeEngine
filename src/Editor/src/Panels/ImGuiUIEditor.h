#pragma once

namespace BeeEngine
{
    class ImGuiUIEditor
    {
    public:
        void SetDefaultStyle();
        void Render();
        void Toggle() { m_IsVisible = !m_IsVisible; }

    private:
        // size_t m_FontSize = 18;
        bool m_IsVisible = false;
    };
} // namespace BeeEngine