#pragma once

#include "Core/TypeDefines.h"
#include "Windowing/WindowProperties.h"

namespace BeeEngine
{
    enum class WindowHandlerAPI
    {
        GLFW = 0,
        SDL = 1
    };
    class WindowHandler
    {
    public:
        virtual ~WindowHandler() = default;
        static WindowHandler* Create(WindowHandlerAPI api, const WindowProperties& properties);
        uint16_t GetWidth() const
        {
            return m_Width;
        }
        uint16_t GetHeight() const
        {
            return m_Height;
        }
        virtual void SetWidth(uint16_t width) = 0;
        virtual void SetHeight(uint16_t height) = 0;
    protected:
        WindowHandler() = default;
        uint16_t m_Width;
        uint16_t m_Height;
        const char *m_Title;
    };
}
