//
// Created by Александр Лебедев on 28.06.2023.
//

#pragma once

#include "SDL3/SDL_events.h"
#include "WindowHandler.h"
#if defined(BEE_COMPILE_SDL)
#include "KeyCodes.h"
#include "SDL3/SDL.h"

namespace BeeEngine::Internal
{
    class SDLWindowHandler : public WindowHandler
    {
    public:
        SDLWindowHandler(const ApplicationProperties& properties, EventQueue& eventQueue);
        ~SDLWindowHandler() override;
        SDLWindowHandler(const SDLWindowHandler&) = delete;
        SDLWindowHandler& operator=(const SDLWindowHandler&) = delete;
        void SetWidth(uint16_t width) override;
        void SetHeight(uint16_t height) override;
        uint64_t GetWindow() override { return (uint64_t)m_Window; }
        void SetVSync(VSync mode) override;

        void HideCursor() override;
        void DisableCursor() override;
        void EnableCursor() override;
        void ShowCursor() override;
        void ProcessEvents() override;
        [[nodiscard]] bool IsRunning() const override;
        Time::secondsD UpdateTime() override;
        void Close() override;

        GraphicsDevice& GetGraphicsDevice() override { return *m_GraphicsDevice; }
        Instance& GetAPIInstance() override { return *m_Instance; }

        WindowNativeInfo GetNativeInfo() override;

        GlobalMouseState GetGlobalMouseState() const override;

    private:
        static Key ConvertKeyCode(SDL_Scancode key);
        static MouseButton ConvertMouseButton(uint8_t button);
        void InitializeVulkan();
        void InitializeWebGPU();
        void HandleDragDropLinux(const SDL_Event& event);

        struct sdlFinalizer
        {
            SDL_Window* window;
            ~sdlFinalizer()
            {
                SDL_DestroyWindow(window);
                SDL_Quit();
            }
        };
        sdlFinalizer m_Finalizer{};
        SDL_Window* m_Window;
        Scope<Instance> m_Instance;
        Scope<GraphicsDevice> m_GraphicsDevice;
        mutable bool m_IsRunning = false;
        mutable bool m_IsClosing = false;

        void InitializeDragDropOnWindows();
    };
} // namespace BeeEngine::Internal
#endif
