//
// Created by Aleksandr on 26.02.2024.
//

#include "VulkanTestLayer.h"

#include "../../../build/Release/CPM-cache/imgui/5943042e244e2c56d422a3fe1c78c63430d6e5cf/imgui.h"

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
