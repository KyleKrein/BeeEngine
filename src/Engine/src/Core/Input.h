//
// Created by alexl on 26.01.2023.
//

#pragma once

#include "Core/Events/EventQueue.h"
#include "KeyCodes.h"
#include "Iterator.h"

namespace BeeEngine
{
    class Input
    {
        friend EventQueue;
    public:
        static bool KeyPressed(Key key);
        static bool MouseKeyPressed(MouseButton button);
        inline static const float GetMouseX()
        {
            return s_MouseX;
        }
        inline static const float GetMouseY()
        {
            return s_MouseY;
        }
        inline static const float GetMouseWheelX()
        {
            return s_MouseWheelX;
        }
        inline static const float GetMouseWheelY()
        {
            return s_MouseWheelY;
        }
    private:
        static void OnEvent(Event* event);
        //inline static void Init();

    private:
        static std::unordered_map<Key, bool> s_Keys;
        static std::unordered_map<MouseButton, bool> s_MouseButtons;
        //static bool s_IsInit;
        //typedef Iterator<MouseButton, MouseButton::Left, MouseButton::Last> MouseIterator;
        //typedef Iterator<Key, Key::Space, Key::LastKey> KeyIterator;

        static float s_MouseX;
        static float s_MouseY;
        static float s_MouseWheelX;
        static float s_MouseWheelY;
    };
}