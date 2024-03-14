//
// Created by Aleksandr on 26.02.2024.
//

#include "VulkanTestLayer.h"

#include "imgui.h"
#include "Platform/Platform.h"

VulkanTestLayer::~VulkanTestLayer()
{
}

void VulkanTestLayer::OnAttach()
{
    Layer::OnAttach();
}

void VulkanTestLayer::OnDetach()
{
    Layer::OnDetach();
}

void VulkanTestLayer::OnUpdate()
{
    Layer::OnUpdate();
}

void VulkanTestLayer::OnGUIRendering()
{
    ImGui::ShowDemoWindow();
}

void VulkanTestLayer::OnEvent(BeeEngine::EventDispatcher& e)
{
    Layer::OnEvent(e);
}
