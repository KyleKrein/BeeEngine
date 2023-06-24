//
// Created by Александр Лебедев on 06.05.2023.
//

#pragma once


#include "BeeEngine.h"
#include "Gui/ImGui/FpsCounter.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/VulkanModel.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

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

    TestLayer(const TestLayer& other) = delete;
    TestLayer& operator=(const TestLayer& other) = delete;
    private:
    BeeEngine::OrthographicCameraController m_CameraController;
    BeeEngine::Internal::FpsCounter m_FpsCounter {};
    BeeEngine::Ref<BeeEngine::Texture2D> m_ForestTexture;
    double currentTime;
    double lastTime;
    double numFrames;

    BeeEngine::Scope<BeeEngine::Internal::VulkanPipeline> m_Pipeline;
    BeeEngine::Scope<BeeEngine::Internal::VulkanModel> m_Model;
    VkPipelineLayout m_PipelineLayout;

    BeeEngine::Internal::VulkanRendererAPI m_RendererAPI;
    void CreatePipelineLayout();
    void CreatePipeline();
    void LoadModels();
};
