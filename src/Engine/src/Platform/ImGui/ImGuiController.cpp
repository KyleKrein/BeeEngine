//
// Created by alexl on 04.06.2023.
//
#include "ImGuiController.h"
#include "../../../Assets/EmbeddedResources.h"
#include "../../Core/Color4.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include <cstddef>

namespace BeeEngine
{
    static void SetupImGuiStyle()
    {
        // Fork of Future Dark style from ImThemes
        ImGuiStyle& style = ImGui::GetStyle();

        style.Alpha = 1.0f;
        style.DisabledAlpha = 1.0f;
        style.WindowPadding = ImVec2(12.0f, 12.0f);
        style.WindowRounding = 0.0f;
        style.WindowBorderSize = 0.0f;
        style.WindowMinSize = ImVec2(20.0f, 20.0f);
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.ChildRounding = 0.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupRounding = 0.0f;
        style.PopupBorderSize = 1.0f;
        style.FramePadding = ImVec2(6.0f, 6.0f);
        style.FrameRounding = 0.0f;
        style.FrameBorderSize = 0.0f;
        style.ItemSpacing = ImVec2(12.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 3.0f);
        style.CellPadding = ImVec2(12.0f, 6.0f);
        style.IndentSpacing = 20.0f;
        style.ColumnsMinSpacing = 6.0f;
        style.ScrollbarSize = 12.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabMinSize = 12.0f;
        style.GrabRounding = 0.0f;
        style.TabRounding = 0.0f;
        style.TabBorderSize = 0.0f;
        style.TabMinWidthForCloseButton = 0.0f;
        style.ColorButtonPosition = ImGuiDir_Right;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled] =
            ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
        style.Colors[ImGuiCol_BorderShadow] =
            ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] =
            ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive] =
            ImVec4(0.5960784554481506f, 0.5585290193557739f, 0.2156862765550613f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] =
            ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarBg] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrab] =
            ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] =
            ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] =
            ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.9763022661209106f, 0.4980392456054688f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] =
            ImVec4(0.4917890429496765f, 0.4935622215270996f, 0.08049511909484863f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] =
            ImVec4(0.6659574508666992f, 0.6781115531921387f, 0.2735729217529297f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] =
            ImVec4(0.5450980663299561f, 0.5276950597763062f, 0.1764705777168274f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] =
            ImVec4(0.5846503376960754f, 0.5960784554481506f, 0.2156862765550613f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_HeaderHovered] =
            ImVec4(0.5245308876037598f, 0.5450980663299561f, 0.1764705777168274f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive] =
            ImVec4(0.5846503376960754f, 0.5960784554481506f, 0.2156862765550613f, 1.0f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
        style.Colors[ImGuiCol_SeparatorHovered] =
            ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
        style.Colors[ImGuiCol_SeparatorActive] =
            ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_ResizeGripHovered] =
            ImVec4(0.5450980663299561f, 0.5371875762939453f, 0.1764705777168274f, 1.0f);
        style.Colors[ImGuiCol_ResizeGripActive] =
            ImVec4(0.5944458842277527f, 0.5960784554481506f, 0.2156862765550613f, 1.0f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
        style.Colors[ImGuiCol_TabUnfocused] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TabUnfocusedActive] =
            ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
        style.Colors[ImGuiCol_PlotLinesHovered] =
            ImVec4(0.9278801679611206f, 0.9803921580314636f, 0.03921570628881454f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogram] =
            ImVec4(0.9055793881416321f, 0.8562365770339966f, 0.2293098121881485f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogramHovered] =
            ImVec4(0.9960784316062927f, 0.9714549779891968f, 0.4745097458362579f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TableBorderStrong] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_TableRowBgAlt] =
            ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
        style.Colors[ImGuiCol_TextSelectedBg] =
            ImVec4(0.5960784554481506f, 0.5781199932098389f, 0.2156862765550613f, 1.0f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.971993625164032f, 1.0f, 0.4980392456054688f, 1.0f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.9763022661209106f, 0.4980392456054688f, 1.0f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.9849196672439575f, 1.0f, 0.4980392456054688f, 1.0f);
        style.Colors[ImGuiCol_NavWindowingDimBg] =
            ImVec4(0.6909871101379395f, 0.6716659665107727f, 0.2817329466342926f, 0.501960813999176f);
        style.Colors[ImGuiCol_ModalWindowDimBg] =
            ImVec4(0.3605113923549652f, 0.3605149984359741f, 0.3605148792266846f, 0.3004291653633118f);
    }

    void BeeEngine::ImGuiController::SetDefaultTheme(float fontSize)
    {
        CreateFontsWithSize(fontSize);
        SetupImGuiStyle();
    }

    void ImGuiController::SetDarkThemeColors()
    {
        auto& colors = ImGui::GetStyle().Colors;

        colors[ImGuiCol_WindowBg] = {0.1f, 0.105f, 0.11f, 1.0f};

        // Headers
        colors[ImGuiCol_Header] = {0.2f, 0.205f, 0.21f, 1.0f};
        colors[ImGuiCol_HeaderHovered] = {0.3f, 0.305f, 0.31f, 1.0f};
        colors[ImGuiCol_HeaderActive] = {0.15f, 0.1505f, 0.151f, 1.0f};

        // Buttons
        colors[ImGuiCol_Button] = {0.2f, 0.205f, 0.21f, 1.0f};
        colors[ImGuiCol_ButtonHovered] = {0.3f, 0.305f, 0.31f, 1.0f};
        colors[ImGuiCol_ButtonActive] = {0.15f, 0.1505f, 0.151f, 1.0f};

        // Frame BG
        colors[ImGuiCol_FrameBg] = {0.2f, 0.205f, 0.21f, 1.0f};
        colors[ImGuiCol_FrameBgHovered] = {0.3f, 0.305f, 0.31f, 1.0f};
        colors[ImGuiCol_FrameBgActive] = {0.15f, 0.1505f, 0.151f, 1.0f};

        // Tabs
        colors[ImGuiCol_Tab] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TabHovered] = {0.38f, 0.3805f, 0.381f, 1.0f};
        colors[ImGuiCol_TabActive] = {0.28f, 0.2805f, 0.281f, 1.0f};
        colors[ImGuiCol_TabUnfocused] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TabUnfocusedActive] = {0.2f, 0.205f, 0.21f, 1.0f};

        // Title
        colors[ImGuiCol_TitleBg] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TitleBgActive] = {0.15f, 0.1505f, 0.151f, 1.0f};
        colors[ImGuiCol_TitleBgCollapsed] = {0.95f, 0.1505f, 0.951f, 1.0f};
    }

    void ImGuiController::CreateFontsWithSize(float fontSize)
    {
        using namespace Internal;
        auto& io = ImGui::GetIO();
        auto scale = WindowHandler::GetInstance()->GetScaleFactor();
        fontSize *= scale;
        auto openSansRegularFont = GetEmbeddedResource(EmbeddedResource::OpenSansRegular);
        auto openSansBoldFont = GetEmbeddedResource(EmbeddedResource::OpenSansBold);

        static const ImWchar iconsRanges[] = {// 0x20, 0xFFFF // full range
                                              0x0020,
                                              0x00FF, // Latin + Suplement
                                              0x0100,
                                              0x017F, // Latin Extended-A
                                              0x0180,
                                              0x024F, // Latin Extended-B
                                              0x0400,
                                              0x052F,
                                              0x2DE0,
                                              0x2DFF,
                                              0xA640,
                                              0xA69F,
                                              0};

        ImFontConfig config{};
        config.SizePixels = fontSize;
        config.FontDataOwnedByAtlas = false;

        io.Fonts->Clear();
        io.FontDefault = io.Fonts->AddFontFromMemoryTTF(
            (void*)openSansRegularFont.data(), openSansRegularFont.size_bytes(), fontSize, &config, iconsRanges);
        io.Fonts->AddFontFromMemoryTTF(
            (void*)openSansBoldFont.data(), openSansBoldFont.size_bytes(), fontSize, &config, iconsRanges);
        // io.Fonts->GetGlyphRangesCyrillic();
        // io.FontGlobalScale = 1.0f/scale; //TODO: uncomment this when dynamic font change is implemented

        io.Fonts->Build();
    }

} // namespace BeeEngine