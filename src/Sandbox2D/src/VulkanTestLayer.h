//
// Created by Aleksandr on 26.02.2024.
//

#pragma once
#include "Core/Layer.h"
#include "Renderer/Renderer.h"

class VulkanTestLayer : public BeeEngine::Layer
{
public:
    ~VulkanTestLayer() override;

    void OnAttach() override;

    void OnDetach() override;

    void OnUpdate(BeeEngine::FrameData& data) override;

    void OnGUIRendering() override;

    void OnEvent(BeeEngine::EventDispatcher& e) override;
};
