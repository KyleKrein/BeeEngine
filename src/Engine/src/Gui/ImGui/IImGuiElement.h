//
// Created by alexl on 01.06.2023.
//

#pragma once
#include "BeeEngine.h"

namespace BeeEngine::Internal
{
    struct IImGuiElement
    {
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void OnEvent(EventDispatcher& event) = 0;
        virtual ~IImGuiElement() = default;
    };
} // namespace BeeEngine::Internal