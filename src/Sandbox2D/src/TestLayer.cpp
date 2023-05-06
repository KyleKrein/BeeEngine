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

}

void TestLayer::OnDetach()
{

}

void TestLayer::OnUpdate()
{
    //BeeTrace("TestLayer::OnUpdate");
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
}


