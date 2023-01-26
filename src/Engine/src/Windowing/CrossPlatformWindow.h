#pragma once

#include "Window.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine{
    class CrossPlatformWindow: public Window
    {
    public:
        virtual ~CrossPlatformWindow() override
        {
            delete m_Handler;
        }
        CrossPlatformWindow(const WindowProperties& properties, EventQueue& eventQueue)
        {
            m_Handler = WindowHandler::Create(WindowHandlerAPI::GLFW, properties, eventQueue);

        }

        uint16_t GetWidth() const override
        {
            return m_Handler->GetWidth();
        }

        uint16_t GetHeight() const override
        {
            return m_Handler->GetHeight();
        }
        void SetWidth(uint16_t width) override
        {
            m_Handler->SetWidth(width);
        }

        void SetHeight(uint16_t height) override
        {
            m_Handler->SetHeight(height);
        }

    private:


    private:
        WindowHandler* m_Handler;
    };
}
