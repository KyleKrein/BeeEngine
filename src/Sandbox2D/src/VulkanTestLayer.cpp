//
// Created by Aleksandr on 26.02.2024.
//

#include "VulkanTestLayer.h"

#include "Platform/Platform.h"
#include "imgui.h"

VulkanTestLayer::~VulkanTestLayer() {}

void VulkanTestLayer::OnAttach()
{
    Layer::OnAttach();
}

void VulkanTestLayer::OnDetach()
{
    Layer::OnDetach();
}

void VulkanTestLayer::OnUpdate(BeeEngine::FrameData& data)
{
    Layer::OnUpdate(data);
}

void VulkanTestLayer::OnGUIRendering()
{
    ImGui::ShowDemoWindow();
}

void VulkanTestLayer::OnEvent(BeeEngine::EventDispatcher& e)
{
    Layer::OnEvent(e);
}
