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
        void PushLayer(Ref<Layer> layer);
        void PushOverlay(Ref<Layer> overlay);
        void PushLayer(Layer&& layer);
        void PushOverlay(Layer&& overlay);
        void PopLayer(Ref<Layer> layer);
        void PopOverlay(Ref<Layer> overlay);
        void SetGuiLayer(ImGuiLayer* guiLayer);
        void FinishGuiRendering();

        void OnEvent(EventDispatcher& dispatcher);
        void Update();
    private:
        std::vector<Ref<Layer>> m_layers;
        Scope<ImGuiLayer> m_guiLayer;
    };
}
