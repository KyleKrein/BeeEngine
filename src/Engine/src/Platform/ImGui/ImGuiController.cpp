//
// Created by alexl on 04.06.2023.
//
#include "ImGuiController.h"
#include "imgui.h"
#include "../../../Assets/EmbeddedResources.h"
#include "../../Core/Color4.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine
{

    void BeeEngine::ImGuiController::SetDefaultTheme(float fontSize)
    {
        using namespace Internal;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        auto OpenSansRegularFont = GetEmbeddedResource(EmbeddedResource::OpenSansRegular);
        auto OpenSansBoldFont = GetEmbeddedResource(EmbeddedResource::OpenSansBold);

        static const ImWchar  icons_ranges[] = { //0x20, 0xFFFF // full range
                                                0x0020, 0x00FF, //Latin + Suplement
                                                0x0100, 0x017F, //Latin Extended-A
                                                0x0180, 0x024F, //Latin Extended-B
                                                 0x0400, 0x052F,
                                                 0x2DE0, 0x2DFF,
                                                 0xA640, 0xA69F,
                                                 0 };

        ImFontConfig config {};
        //config.MergeMode = true;
        auto scale = WindowHandler::GetInstance()->GetScaleFactor();
        config.SizePixels = fontSize * scale;

        io.FontDefault = io.Fonts->AddFontFromMemoryTTF((void *) OpenSansRegularFont.data(), OpenSansRegularFont.size_bytes(), fontSize, &config, icons_ranges);
        io.Fonts->AddFontFromMemoryTTF((void *) OpenSansBoldFont.data(), OpenSansBoldFont.size_bytes(), fontSize, &config, icons_ranges);
        //io.Fonts->GetGlyphRangesCyrillic();
        //io.FontGlobalScale = 1.0f/scale; //TODO: uncomment this when dynamic font change is implemented

        style.WindowMinSize.x = 370.0f;
        io.Fonts->Build();

        style.FrameRounding = 4.0f;
        style.WindowRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;
        style.WindowBorderSize = 0.0f;
        style.FrameBorderSize = 0.0f;
        style.PopupBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(8.0f, 8.0f);
        style.ItemSpacing = ImVec2(8.0f, 8.0f);
        style.ItemInnerSpacing = ImVec2(8.0f, 8.0f);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.GrabMinSize = 10.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_Right;
        style.DisplaySafeAreaPadding = ImVec2(0.0f, 0.0f);
        style.DisplayWindowPadding = ImVec2(0.0f, 0.0f);
        style.DisplaySafeAreaPadding = ImVec2(0.0f, 0.0f);
        style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
        style.AntiAliasedFill = true;
        style.AntiAliasedLines = true;
        style.CurveTessellationTol = 1.25f;
    }

    void ImGuiController::SetDarkThemeColors()
    {
        auto& colors = ImGui::GetStyle().Colors;

        colors[ImGuiCol_WindowBg] = {0.1f, 0.105f, 0.11f, 1.0f};

        //Headers
        colors[ImGuiCol_Header] = {0.2f, 0.205f, 0.21f, 1.0f};
        colors[ImGuiCol_HeaderHovered] = {0.3f, 0.305f, 0.31f, 1.0f};
        colors[ImGuiCol_HeaderActive] = {0.15f, 0.1505f, 0.151f, 1.0f};

        //Buttons
        colors[ImGuiCol_Button] = {0.2f, 0.205f, 0.21f, 1.0f};
        colors[ImGuiCol_ButtonHovered] = {0.3f, 0.305f, 0.31f, 1.0f};
        colors[ImGuiCol_ButtonActive] = {0.15f, 0.1505f, 0.151f, 1.0f};

        //Frame BG
        colors[ImGuiCol_FrameBg] = {0.2f, 0.205f, 0.21f, 1.0f};
        colors[ImGuiCol_FrameBgHovered] = {0.3f, 0.305f, 0.31f, 1.0f};
        colors[ImGuiCol_FrameBgActive] = {0.15f, 0.1505f, 0.151f, 1.0f};

        //Tabs
        colors[ImGuiCol_Tab] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TabHovered] = {0.38f, 0.3805f, 0.381f, 1.0f};
        colors[ImGuiCol_TabActive] = {0.28f, 0.2805f, 0.281f, 1.0f};
        colors[ImGuiCol_TabUnfocused] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TabUnfocusedActive] = {0.2f, 0.205f, 0.21f, 1.0f};

        //Title
        colors[ImGuiCol_TitleBg] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TitleBgActive] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TitleBgCollapsed] = {0.95f, 0.1505f, 0.951f, 1.0f};
    }


}