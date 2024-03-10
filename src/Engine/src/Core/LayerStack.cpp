//
// Created by alexl on 26.01.2023.
//

#include "LayerStack.h"
#include "Application.h"
#include "Renderer/Renderer.h"
#include <ranges>


namespace BeeEngine{

    LayerStack::LayerStack()
    :m_layers()
    {

    }

    LayerStack::~LayerStack()
    {
        BEE_PROFILE_FUNCTION();
        for (auto& layer: m_layers)
        {
            layer->OnDetach();
        }
        m_guiLayer->OnDetach();
    }

    void LayerStack::PushLayer(Ref<Layer> layer)
    {
        BEE_PROFILE_FUNCTION();
        m_layers.insert(m_layers.begin(), layer);
        layer->OnAttach();
    }

    void LayerStack::PushOverlay(Ref<Layer> overlay)
    {
        BEE_PROFILE_FUNCTION();
        m_layers.push_back(overlay);
        overlay->OnAttach();
    }

    void LayerStack::PushLayer(Layer&& layer)
    {
        BEE_PROFILE_FUNCTION();
        auto layerPtr = CreateRef<Layer>(layer);
        m_layers.insert(m_layers.begin(), layerPtr);
        layerPtr->OnAttach();
    }

    void LayerStack::PushOverlay(Layer&& overlay)
    {
        BEE_PROFILE_FUNCTION();
        auto layerPtr = CreateRef<Layer>(overlay);
        m_layers.push_back(layerPtr);
        layerPtr->OnAttach();
    }

    void LayerStack::PopLayer(Ref<Layer> layer)
    {
        BEE_PROFILE_FUNCTION();
        auto it = std::remove(m_layers.begin(), m_layers.end(), layer);
        m_layers.erase(it, m_layers.end());
        layer->OnDetach();
    }

    void LayerStack::PopOverlay(Ref<Layer> overlay)
    {
        BEE_PROFILE_FUNCTION();
        auto it = std::remove(m_layers.begin(), m_layers.end(), overlay);
        m_layers.erase(it, m_layers.end());
        overlay->OnDetach();
    }

    void LayerStack::OnEvent(EventDispatcher &dispatcher)
    {
        using std::views::reverse;
        BEE_PROFILE_FUNCTION();
        for (auto& layer : m_layers | reverse)
        {
            layer->OnEvent(dispatcher);
            if(dispatcher.IsHandled())
            {
                break;
            }
        }
    }

    void LayerStack::Update()
    {
        BEE_PROFILE_FUNCTION();
        if (!Application::GetInstance().IsMinimized())
        {
            {
                BEE_PROFILE_SCOPE("Layers::Update");
                for (auto& layer: m_layers)
                {
                    layer->OnUpdate();
                }
            }
            {
                BEE_PROFILE_SCOPE("Layers::Renderer::Flush");
                Renderer::FinalFlush();
            }
            {
                BEE_PROFILE_SCOPE("Layers::GUIRendering");
                m_guiLayer->OnBegin();
                for (auto& layer: m_layers)
                {
                    layer->OnGUIRendering();
                }
                m_guiLayer->OnGUIRendering();
                m_guiLayer->OnEnd();
            }
        }
    }

    void LayerStack::SetGuiLayer(ImGuiLayer* guiLayer)
    {
        m_guiLayer.reset(guiLayer);
    }

    void LayerStack::FinishGuiRendering()
    {
        m_guiLayer->OnEnd();
    }
}
