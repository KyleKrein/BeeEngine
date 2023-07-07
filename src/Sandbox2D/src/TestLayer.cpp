//
// Created by Александр Лебедев on 06.05.2023.
//

#include <sstream>
#include "TestLayer.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "Utils/ModelLoader.h"
#include "ext/matrix_transform.hpp"
#include "ext/matrix_clip_space.hpp"
#include "Renderer/BufferLayout.h"
#include "Platform/Vulkan/VulkanMaterial.h"

TestLayer::TestLayer()
: m_RendererAPI(BeeEngine::Internal::VulkanRendererAPI::GetInstance())
{
}

TestLayer::~TestLayer()
= default;

void TestLayer::OnAttach()
{
    //BeeEngine::Renderer::SetClearColor(BeeEngine::Color4::CornflowerBlue);
    //m_CameraController = BeeEngine::OrthographicCameraController();
    //m_ForestTexture = BeeEngine::Texture2D::Create("Assets/Textures/forest.png");
    LoadModels();
    CreatePipelineLayout();
    CreatePipeline();

    BeeEngine::MeshComponent monkey;
    monkey.Mesh = GetMesh("monkey");
    monkey.Material = GetMaterial("defaultmaterial");
    m_MeshComponents.push_back(std::move(monkey));
}

void TestLayer::OnDetach()
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
    vkDestroyPipelineLayout(device.GetDevice(), m_PipelineLayout, nullptr);
}
struct Push
{
    glm::mat4 transform;
};

void TestLayer::OnUpdate()
{
    //m_CameraController.OnUpdate();
    //BeeEngine::Renderer::Clear();
    //BeeEngine::Renderer2D::BeginScene(m_CameraController);

    //BeeEngine::Renderer2D::DrawRectangle(0,0,0.1,1,1, BeeEngine::Color4::Red);
    /*
    for (int i = 0; i < 24*5; ++i)
    {
        for (int j = 0; j < 30*5; ++j)
        {
            //BeeEngine::Renderer2D::DrawImage(j, i, 0.0f, 1,1, m_ForestTexture);
        }
    }*/
    m_FpsCounter.Update();
    //return;
    frame++;
    if(m_RendererAPI.IsFrameStarted())
    /*{
        auto commandBuffer = m_RendererAPI.GetCurrentCommandBuffer();
        DrawMeshComponents(commandBuffer, m_MeshComponents);
    }
    return;*/
    {
        auto commandBuffer = m_RendererAPI.GetCurrentCommandBuffer();
        //m_RendererAPI.BeginSwapchainRenderPass(commandBuffer);
        m_Pipeline->Bind(commandBuffer);
        m_MonkeyModel->Bind(commandBuffer);
        //make a model view matrix for rendering the object
        //camera position
        glm::vec3 camPos = { 0.f,0.f,-2.f };

        glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
        //camera projection
        glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
        projection[1][1] *= -1;
        //model rotation
        glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(frame * 0.4f), glm::vec3(0, 1, 0));

        //calculate final mesh matrix
        glm::mat4 mesh_matrix = projection * view * model;

        Push constants;
        constants.transform = mesh_matrix;
        //upload the matrix to the GPU via push constants
        vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Push), &constants);

        //m_Model->Bind(commandBuffer);
        //m_Model->Draw(commandBuffer);
        m_MonkeyModel->Draw(commandBuffer);
        //m_RendererAPI.EndSwapchainRenderPass(commandBuffer);
    }
    //BeeEngine::Renderer2D::EndScene();

}

void TestLayer::OnGUIRendering()
{
    m_FpsCounter.Render();
    //ImGui::ShowDemoWindow();
}
static bool ResizeEvent(BeeEngine::WindowResizeEvent& event)
{
    BeeInfo("Window resized to {0}x{1}", event.GetWidth(), event.GetHeight());
    return true;
}
void TestLayer::OnEvent(BeeEngine::EventDispatcher &e)
{
    //e.Dispatch<BeeEngine::WindowResizeEvent&, BeeEngine::EventType::WindowResize>(ResizeEvent);
    //m_CameraController.OnEvent(e);
}
void TestLayer::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Push);

    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout((*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice()).GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
    {
        BeeError("Failed to create pipeline layout");
    }
}

void TestLayer::CreatePipeline()
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
    BeeEngine::Internal::PipelineConfigInfo pipelineConfig{};
    BeeEngine::Internal::VulkanPipeline::DefaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = m_RendererAPI.GetSwapchainRenderPass();
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    pipelineConfig.bindingDescriptions = m_MonkeyModel->GetBindingDescriptions();
    pipelineConfig.attributeDescriptions = m_MonkeyModel->GetAttributeDescriptions();
    m_Pipeline = BeeEngine::CreateScope<BeeEngine::Internal::VulkanPipeline>((*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice()).GetDevice(),
                                                                             "Shaders/VulkanTestShader.vert","Shaders/VulkanTestShader.frag", pipelineConfig);
    CreateMaterial(m_Pipeline->GetHandle(), m_PipelineLayout, "defaultmaterial");
}

void TestLayer::LoadModels()
{
    auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
    BeeEngine::BufferLayout layout = {
            {BeeEngine::ShaderDataType::Float3, "a_Position"},
            {BeeEngine::ShaderDataType::Float3, "a_Normal"},
            {BeeEngine::ShaderDataType::Float3, "a_Color"}
    };

    std::vector<BeeEngine::Vertex> vertices = {
            {{0.0f, -0.5f , 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f , 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f , 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };
#if 0
    m_Model = BeeEngine::CreateRef<BeeEngine::Internal::VulkanModel>(device,
                                                                       gsl::span<BeeEngine::byte>{(BeeEngine::byte*)vertices.data(),
                                                                                                  vertices.size() * sizeof(BeeEngine::Vertex)},
                                                                       vertices.size(),
                                                                       layout);

    std::vector<BeeEngine::Vertex> monkeyVertices;
    bool result = BeeEngine::Internal::ModelLoader::LoadObj("Assets/monkey_smooth.obj", monkeyVertices);
    if (!result)
    {
        BeeError("Failed to load model");
    }
    m_MonkeyModel = BeeEngine::CreateRef<BeeEngine::Internal::VulkanModel>(device,
                                                                  gsl::span<BeeEngine::byte>{(BeeEngine::byte*)monkeyVertices.data(),
                                                                                                        monkeyVertices.size() * sizeof(BeeEngine::Vertex)},
                                                                                                        monkeyVertices.size(),
                                                                                                        layout);
#endif
    m_Meshes["monkey"] = m_MonkeyModel;
    m_Meshes["triangle"] = m_Model;
}

BeeEngine::Ref<BeeEngine::Material>
TestLayer::CreateMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout, BeeEngine::in<std::string> name)
{
    BeeEngine::Ref<BeeEngine::Internal::VulkanMaterial> material = BeeEngine::CreateRef<BeeEngine::Internal::VulkanMaterial>(pipeline, pipelineLayout);
    m_Materials[name] = material;
    return material;
}

BeeEngine::Ref<BeeEngine::Material> TestLayer::GetMaterial(BeeEngine::in<std::string> name)
{
    auto it = m_Materials.find(name);
    if (it == m_Materials.end()) {
        BeeCoreError("Material {0} not found", name);
        return nullptr;
    }
    else {
        return (*it).second;
    }
}

BeeEngine::Ref<BeeEngine::Mesh> TestLayer::GetMesh(BeeEngine::in<std::string> name)
{
    auto it = m_Meshes.find(name);
    if (it == m_Meshes.end()) {
        BeeCoreError("Mesh {0} not found", name);
        return nullptr;
    }
    else {
        return (*it).second;
    }
}

void TestLayer::DrawMeshComponents(VkCommandBuffer cmd, std::vector<BeeEngine::MeshComponent> components)
{
    //make a model view matrix for rendering the object
    //camera view
    glm::vec3 camPos = { 0.f,-6.f,-10.f };

    glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
    //camera projection
    glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
    projection[1][1] *= -1;

    BeeEngine::Mesh* lastMesh = nullptr;
    BeeEngine::Material* lastMaterial = nullptr;
    for (int i = 0; i < components.size(); i++)
    {
        BeeEngine::MeshComponent& object = components[i];

        //only bind the pipeline if it doesn't match with the already bound one
        if (object.Material.get() != lastMaterial) {

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ((BeeEngine::Internal::VulkanMaterial*)object.Material.get())->Pipeline);
            lastMaterial = object.Material.get();
        }


        glm::mat4 model{1.0f};//= object.transformMatrix;
        //final render matrix, that we are calculating on the cpu
        glm::mat4 mesh_matrix = projection * view * model;

        Push constants;
        constants.transform = mesh_matrix;

        //upload the mesh to the GPU via push constants
        vkCmdPushConstants(cmd, ((BeeEngine::Internal::VulkanMaterial*)object.Material.get())->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Push), &constants);

        //only bind the mesh if it's a different one from last bind
        if (object.Mesh.get() != lastMesh) {
            //bind the mesh vertex buffer with offset 0
            VkDeviceSize offset = 0;
            ((BeeEngine::Internal::VulkanModel*)object.Mesh.get())->Bind(cmd);
            lastMesh = object.Mesh.get();
        }
        //we can now draw
        ((BeeEngine::Internal::VulkanModel*)object.Mesh.get())->Draw(cmd);
    }
}
