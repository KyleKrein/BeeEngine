//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once
#include "BeeEngine.h"

class WebGPUTestLayer: public BeeEngine::Layer
{
public:
    WebGPUTestLayer()
    {

    }

    ~WebGPUTestLayer() override
    {

    }

    void OnAttach() override
    {
        Layer::OnAttach();
    }

    void OnDetach() override
    {
        Layer::OnDetach();
    }

    void OnUpdate() override
    {
        Layer::OnUpdate();
    }

    void OnGUIRendering() override {}
    void OnEvent(BeeEngine::EventDispatcher& e) override {}
};
