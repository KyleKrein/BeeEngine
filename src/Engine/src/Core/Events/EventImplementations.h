//
// Created by alexl on 26.01.2023.
//

#pragma once
#include "Core/Events/Event.h"
namespace BeeEngine
{
    struct WindowCloseEvent: public Event
    {
    public:
        WindowCloseEvent()
        {
            Category = EventCategory::App;
        }
    };
}