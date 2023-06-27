//
// Created by Александр Лебедев on 06.05.2023.
//

#pragma once


#include "BeeEngine.h"
#include "Gui/ImGui/FpsCounter.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/VulkanModel.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Renderer/Material.h"
#include "Scene/Components.h"

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

    BeeEngine::Ref<BeeEngine::Internal::VulkanModel> m_MonkeyModel;

    BeeEngine::Scope<BeeEngine::Internal::VulkanPipeline> m_Pipeline;
    BeeEngine::Ref<BeeEngine::Internal::VulkanModel> m_Model;
    VkPipelineLayout m_PipelineLayout;

    std::vector<BeeEngine::MeshComponent> m_MeshComponents;

    BeeEngine::Internal::VulkanRendererAPI& m_RendererAPI;
    void CreatePipelineLayout();
    void CreatePipeline();
    void LoadModels();
    uint32_t frame = 0;

    std::unordered_map<std::string, BeeEngine::Ref<BeeEngine::Material>> m_Materials;
    std::unordered_map<std::string,BeeEngine::Ref<BeeEngine::Mesh>> m_Meshes;

    BeeEngine::Ref<BeeEngine::Material> CreateMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, BeeEngine::in<std::string> name);

    BeeEngine::Ref<BeeEngine::Material> GetMaterial(BeeEngine::in<std::string> name);
    BeeEngine::Ref<BeeEngine::Mesh> GetMesh(BeeEngine::in<std::string> name);

    void DrawMeshComponents(VkCommandBuffer cmd, std::vector<BeeEngine::MeshComponent> components);
};
