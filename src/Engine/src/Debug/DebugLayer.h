//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Core/Layer.h"

namespace BeeEngine
{
    class DebugLayer: public Layer
    {
    public:
        DebugLayer() {};
        ~DebugLayer() override {};
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate() override;
        void OnGUIRendering() override;
        void OnEvent(EventDispatcher& e) override;
    };
}