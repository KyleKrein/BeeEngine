//
// Created by Александр Лебедев on 06.05.2023.
//

#include "TestLayer.h"

TestLayer::TestLayer()
{

}

TestLayer::~TestLayer()
{

}

void TestLayer::OnAttach()
{
    BeeEngine::Renderer::SetClearColor(BeeEngine::Color4::CornflowerBlue);
    m_CameraController = BeeEngine::OrthographicCameraController();
}

void TestLayer::OnDetach()
{

}

void TestLayer::OnUpdate()
{
    m_CameraController.OnUpdate();
    BeeEngine::Renderer::Clear();
    BeeEngine::Renderer2D::BeginScene(m_CameraController);

    //BeeTrace("TestLayer::OnUpdate");
    if (BeeEngine::Input::KeyPressed(BeeEngine::Key::Tab))
        BeeInfo("Tab key is pressed (poll)!");




    BeeEngine::Renderer2D::DrawRectangle(0,0,0.1,1,1, BeeEngine::Color4::Red);

    BeeEngine::Renderer2D::EndScene();

}

void TestLayer::OnGUIRendering()
{

}
static bool ResizeEvent(BeeEngine::WindowResizeEvent& event)
{
    BeeInfo("Window resized to {0}x{1}", event.GetWidth(), event.GetHeight());
    return true;
}
void TestLayer::OnEvent(BeeEngine::EventDispatcher &e)
{
    e.Dispatch<BeeEngine::WindowResizeEvent&, BeeEngine::EventType::WindowResize>(ResizeEvent);
    m_CameraController.OnEvent(e);
}


