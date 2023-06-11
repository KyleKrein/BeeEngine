//
// Created by Александр Лебедев on 06.05.2023.
//

#include <sstream>
#include "TestLayer.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
TestLayer::TestLayer()
: m_Pipeline((*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice()).GetDevice(),
             "Shaders/test.vert","Shaders/test.frag", BeeEngine::Internal::VulkanPipeline::DefaultPipelineConfigInfo(1280, 720))
{

}

TestLayer::~TestLayer()
= default;

void TestLayer::OnAttach()
{;
    //BeeEngine::Renderer::SetClearColor(BeeEngine::Color4::CornflowerBlue);
    //m_CameraController = BeeEngine::OrthographicCameraController();
    //m_ForestTexture = BeeEngine::Texture2D::Create("Assets/Textures/forest.png");
}

void TestLayer::OnDetach()
{

}

void TestLayer::OnUpdate()
{
    //m_CameraController.OnUpdate();
    //BeeEngine::Renderer::Clear();
    //BeeEngine::Renderer2D::BeginScene(m_CameraController);

    //BeeEngine::Renderer2D::DrawRectangle(0,0,0.1,1,1, BeeEngine::Color4::Red);
    for (int i = 0; i < 24*5; ++i)
    {
        for (int j = 0; j < 30*5; ++j)
        {
            //BeeEngine::Renderer2D::DrawImage(j, i, 0.0f, 1,1, m_ForestTexture);
        }
    }
    currentTime = glfwGetTime();
    double delta = currentTime - lastTime;

    if (delta >= 1) {
        int framerate{ std::max(1, int(numFrames / delta)) };
        std::stringstream title;
        title << "Running at " << framerate << " fps.";
        glfwSetWindowTitle((GLFWwindow*)BeeEngine::WindowHandler::GetInstance()->GetWindow(), title.str().c_str());
        lastTime = currentTime;
        numFrames = -1;
        framerate = float(1000.0 / framerate);
    }

    ++numFrames;
    //BeeEngine::Renderer2D::EndScene();
    //m_FpsCounter.Update();
}

void TestLayer::OnGUIRendering()
{
    //m_FpsCounter.Render();
}
static bool ResizeEvent(BeeEngine::WindowResizeEvent& event)
{
    BeeInfo("Window resized to {0}x{1}", event.GetWidth(), event.GetHeight());
    return true;
}
void TestLayer::OnEvent(BeeEngine::EventDispatcher &e)
{
    e.Dispatch<BeeEngine::WindowResizeEvent&, BeeEngine::EventType::WindowResize>(ResizeEvent);
    //m_CameraController.OnEvent(e);
}


