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
        void PushLayer(Layer& layer);
        void PushOverlay(Layer& overlay);
        void PopLayer(Layer& layer);
        void PopOverlay(Layer& overlay);
        void SetGuiLayer(ImGuiLayer* guiLayer);

        void OnEvent(EventDispatcher& dispatcher);
        void Update();
    private:
        std::vector<Layer*> m_layers;
        ImGuiLayer* m_guiLayer;
    };
}
