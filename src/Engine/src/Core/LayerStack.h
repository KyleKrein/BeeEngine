//
// Created by alexl on 26.01.2023.
//

#pragma once

#include "Layer.h"

namespace BeeEngine{
    struct LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();
        inline void PushLayer(Layer& layer);
        inline void PushOverlay(Layer& overlay);
        inline void PopLayer(Layer& layer);
        inline void PopOverlay(Layer& overlay);
        void SetGuiLayer(const ImGuiLayer& guiLayer);

        void OnEvent(EventDispatcher& dispatcher);
        void Update();
    private:
        std::vector<Layer*> m_layers;
        ImGuiLayer m_guiLayer;
    };
}
