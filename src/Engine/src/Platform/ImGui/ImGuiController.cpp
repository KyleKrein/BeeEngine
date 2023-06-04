//
// Created by alexl on 04.06.2023.
//
#include "ImGuiController.h"
#include "imgui.h"
#include "../../../Assets/EmbeddedResources.h"
namespace BeeEngine
{

    void BeeEngine::ImGuiController::SetDefaultTheme()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        //io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/BeeEngine/Assets/OpenSans/OpenSans-VariableFont_wdth,wght.ttf", 18.0f);
        io.FontDefault = io.Fonts->AddFontFromMemoryTTF((void *) gOpenSansData, gOpenSansSize, 24.0f);

    }


}