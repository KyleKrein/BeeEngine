#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine{
    class Window
    {
    public:
        virtual ~Window() {};
        static Window& Create(const String& title, uint16_t width, uint16_t height);
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
        uint16_t m_Width;
        uint16_t m_Height;
    };
}
