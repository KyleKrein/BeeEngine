//
// Created by alexl on 26.01.2023.
//

#include "LayerStack.h"
#include "Application.h"


namespace BeeEngine{

    LayerStack::LayerStack()
    :m_layers()
    {

    }

    LayerStack::~LayerStack()
    {
        for (auto layer: m_layers)
        {
            layer->OnDetach();
            delete layer;
        }
        m_guiLayer->OnDetach();
        delete m_guiLayer;
    }

    void LayerStack::PushLayer(Layer &layer)
    {
        m_layers.insert(m_layers.begin(), &layer);
        layer.OnAttach();
    }

    void LayerStack::PushOverlay( Layer &overlay)
    {
        m_layers.push_back(&overlay);
        overlay.OnAttach();
    }

    void LayerStack::PopLayer(Layer &layer)
    {
        std::remove(m_layers.begin(), m_layers.end(), &layer);
        layer.OnDetach();
    }

    void LayerStack::PopOverlay(Layer &overlay)
    {
        std::remove(m_layers.begin(), m_layers.end(), &overlay);
        overlay.OnDetach();
    }

    void LayerStack::OnEvent(EventDispatcher &dispatcher)
    {
        for (auto layer = m_layers.rbegin(); layer < m_layers.rend(); ++layer)
        {
            (*layer)->OnEvent(dispatcher);
            if(dispatcher.IsHandled())
            {
                break;
            }
        }
    }

    void LayerStack::Update()
    {
        if (!Application::GetInstance()->IsMinimized())
        {
            for (auto layer: m_layers)
            {
                layer->OnUpdate();
            }

            m_guiLayer->OnBegin();
            for (auto layer: m_layers)
            {
                layer->OnGUIRendering();
            }
            m_guiLayer->OnGUIRendering();
            m_guiLayer->OnEnd();
        }
    }

    void LayerStack::SetGuiLayer(ImGuiLayer &guiLayer)
    {
        m_guiLayer = &guiLayer;
    }
}
