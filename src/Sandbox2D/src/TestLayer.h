//
// Created by Александр Лебедев on 06.05.2023.
//

#pragma once


#include "BeeEngine.h"

class TestLayer: public BeeEngine::Layer
{
public:
    TestLayer();
    ~TestLayer() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnGUIRendering() override;
    void OnEvent(BeeEngine::EventDispatcher& e) override;
    private:
    BeeEngine::OrthographicCameraController m_CameraController;
};
