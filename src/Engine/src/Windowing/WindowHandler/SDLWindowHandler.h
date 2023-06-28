//
// Created by Александр Лебедев on 28.06.2023.
//

#pragma once


#include "WindowHandler.h"
#include "SDL.h"
#include "KeyCodes.h"

namespace BeeEngine::Internal
{
    class SDLWindowHandler: public WindowHandler
    {
    public:
        SDLWindowHandler(const WindowProperties& properties, EventQueue& eventQueue);
        ~SDLWindowHandler() override;
        SDLWindowHandler(const SDLWindowHandler&) = delete;
        SDLWindowHandler& operator=(const SDLWindowHandler&) = delete;
        void SetWidth(uint16_t width) override;
        void SetHeight(uint16_t height) override;
        uint64_t GetWindow() override
        {
            return (uint64_t)m_Window;
        }
        void SetVSync(VSync mode) override;

        void HideCursor() override;
        void DisableCursor() override;
        void ShowCursor() override;
        void ProcessEvents() override;
        void SwapBuffers() override;
        void MakeContextCurrent() override;
        void MakeContextNonCurrent() override;
        [[nodiscard]] bool IsRunning() const override;
        void UpdateTime() override;
        void Close() override;

        GraphicsDevice& GetGraphicsDevice() override
        {
            return *m_GraphicsDevice;
        }
        Instance& GetAPIInstance() override
        {
            return *m_Instance;
        }

    private:
        static Key ConvertKeyCode(SDL_Scancode key);
        static MouseButton ConvertMouseButton(uint8_t button);
        void InitializeVulkan();


        struct sdlFinalizer
        {
            SDL_Window *window;
            ~sdlFinalizer()
            {
                SDL_DestroyWindow(window);
                SDL_Quit();
            }
        };
        sdlFinalizer m_Finalizer;
        SDL_Window* m_Window;
        Scope<Instance> m_Instance;
        Scope<GraphicsDevice> m_GraphicsDevice;
        mutable bool m_IsRunning = false;
        mutable bool m_IsClosing = false;
    };
}
