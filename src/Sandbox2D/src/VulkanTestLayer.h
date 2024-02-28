//
// Created by Aleksandr on 26.02.2024.
//

#pragma once
#include "Core/Layer.h"


class VulkanTestLayer: public BeeEngine::Layer
{
public:
    ~VulkanTestLayer() override;

    void OnAttach() override;

    void OnDetach() override;

    void OnUpdate() override;

    void OnGUIRendering() override;

    void OnEvent(BeeEngine::EventDispatcher& e) override;
};
