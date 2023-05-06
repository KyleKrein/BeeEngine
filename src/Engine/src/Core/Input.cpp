//
// Created by alexl on 26.01.2023.
//

#include "Input.h"


namespace BeeEngine
{
    bool Input::s_IsInit = false;

    bool Input::KeyPressed(Key key)
    {
        return s_Keys[key];
    }

    bool Input::MouseKeyPressed(MouseButton button)
    {
        return s_MouseButtons[button];
    }

    void Input::OnEvent(Event* event)
    {
        if (!(event->Category & EventCategory::Input))
            return;
        //TODO: Finish Input on event
    }

    void Input::Init()
    {
        s_IsInit = true;
    }
}
