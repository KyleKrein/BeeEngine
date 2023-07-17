//
// Created by alexl on 17.07.2023.
//

#pragma once

#include "IImGuiElement.h"

namespace BeeEngine::Internal
{
    class RendererStatisticsGUI final: public IImGuiElement
    {
    public:
        void Update() override{};
        void Render() override;
        void OnEvent(EventDispatcher& event) override{};
        ~RendererStatisticsGUI() override = default;
    };
}
