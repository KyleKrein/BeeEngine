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
        inline static bool KeyPressed(Key key);
        inline static bool MouseKeyPressed(MouseButton button);
    private:
        inline static void OnEvent(Event& event);
        inline static void Init();

    private:
        static std::unordered_map<Key, bool> s_Keys;
        static std::unordered_map<MouseButton, bool> s_MouseButtons;
        static bool s_IsInit;
        typedef Iterator<MouseButton, MouseButton::Left, MouseButton::Last> MouseIterator;
        typedef Iterator<Key, Key::Space, Key::LastKey> KeyIterator;
    };
}
