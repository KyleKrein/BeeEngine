//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once
#include "BeeEngine.h"
#include "Gui/ImGui/FpsCounter.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Mesh.h"
#include "Renderer/InstancedBuffer.h"
#include "Renderer/AssetManager.h"

class WebGPUTestLayer: public BeeEngine::Layer
{
public:
    WebGPUTestLayer() = default;

    ~WebGPUTestLayer() override = default;

    void OnAttach() override;

    void OnDetach() override;

    void OnUpdate() override;

    void OnGUIRendering() override;
    void OnEvent(BeeEngine::EventDispatcher& e) override {}

    private:
    BeeEngine::Internal::FpsCounter m_FpsCounter;
    BeeEngine::AssetManager m_AssetManager;
    BeeEngine::InstancedBuffer* m_InstancedBuffer;
    BeeEngine::Model* m_Model;
};
