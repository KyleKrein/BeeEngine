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
#include "Renderer/UniformBuffer.h"

class WebGPUTestLayer: public BeeEngine::Layer
{
public:
    WebGPUTestLayer() = default;

    ~WebGPUTestLayer() override = default;

    void OnAttach() override;

    void OnDetach() override;

    void OnUpdate() override;

    void OnGUIRendering() override;
    void OnEvent(BeeEngine::EventDispatcher& e) override {
        m_CameraController.OnEvent(e);
    }

    private:
    BeeEngine::Internal::FpsCounter m_FpsCounter;
    BeeEngine::AssetManager m_AssetManager;
    BeeEngine::InstancedBuffer* m_InstancedBuffer;
    BeeEngine::Ref<BeeEngine::UniformBuffer> m_CameraUniformBuffer;
    BeeEngine::Model* m_Model;
    BeeEngine::Texture2D* m_ForestTexture;
    BeeEngine::Ref<BeeEngine::Texture2D> m_BlankTexture;
    BeeEngine::OrthographicCameraController m_CameraController;
    BeeEngine::Ref<BeeEngine::BindingSet> m_ModelBindingSet;
    BeeEngine::Ref<BeeEngine::BindingSet> m_ForestTextureBindingSet;
    BeeEngine::Ref<BeeEngine::BindingSet> m_BlankTextureBindingSet;
};
