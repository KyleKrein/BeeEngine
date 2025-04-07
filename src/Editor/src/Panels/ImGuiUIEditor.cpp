#include "ImGuiUIEditor.h"
#include "Core/ToString.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Platform/ImGui/ImGuiController.h"
#include <sstream>
namespace BeeEngine
{
    void ImGuiUIEditor::SetDefaultStyle()
    {
        ImGuiController::SetDefaultTheme();
    }
    String ConvertStyleToCpp()
    {
        std::ostringstream oss;
        oss << "auto& style = ImGui::GetStyle();\n";
        oss << "style.Alpha = " << ImGui::GetStyle().Alpha << "f;\n";
        oss << "style.DisabledAlpha = " << ImGui::GetStyle().DisabledAlpha << "f;\n";
        oss << "style.WindowPadding = ImVec2(" << ImGui::GetStyle().WindowPadding.x << "f, "
            << ImGui::GetStyle().WindowPadding.y << "f);\n";
        oss << "style.WindowRounding = " << ImGui::GetStyle().WindowRounding << "f;\n";
        oss << "style.WindowBorderSize = " << ImGui::GetStyle().WindowBorderSize << "f;\n";
        oss << "style.WindowMinSize = ImVec2(" << ImGui::GetStyle().WindowMinSize.x << "f, "
            << ImGui::GetStyle().WindowMinSize.y << "f);\n";
        oss << "style.WindowTitleAlign = ImVec2(" << ImGui::GetStyle().WindowTitleAlign.x << "f, "
            << ImGui::GetStyle().WindowTitleAlign.y << "f);\n";
        oss << "style.WindowMenuButtonPosition = " << ToString(ImGui::GetStyle().WindowMenuButtonPosition) << ";\n";
        oss << "style.ChildRounding = " << ImGui::GetStyle().ChildRounding << "f;\n";
        oss << "style.ChildBorderSize = " << ImGui::GetStyle().ChildBorderSize << "f;\n";
        oss << "style.PopupRounding = " << ImGui::GetStyle().PopupRounding << "f;\n";
        oss << "style.PopupBorderSize = " << ImGui::GetStyle().PopupBorderSize << "f;\n";
        oss << "style.FramePadding = ImVec2(" << ImGui::GetStyle().FramePadding.x << "f, "
            << ImGui::GetStyle().FramePadding.y << "f);\n";
        oss << "style.FrameRounding = " << ImGui::GetStyle().FrameRounding << "f;\n";
        oss << "style.FrameBorderSize = " << ImGui::GetStyle().FrameBorderSize << "f;\n";
        oss << "style.ItemSpacing = ImVec2(" << ImGui::GetStyle().ItemSpacing.x << "f, "
            << ImGui::GetStyle().ItemSpacing.y << "f);\n";
        oss << "style.ItemInnerSpacing = ImVec2(" << ImGui::GetStyle().ItemInnerSpacing.x << "f, "
            << ImGui::GetStyle().ItemInnerSpacing.y << "f);\n";
        oss << "style.CellPadding = ImVec2(" << ImGui::GetStyle().CellPadding.x << "f, "
            << ImGui::GetStyle().CellPadding.y << "f);\n";
        oss << "style.TouchExtraPadding = ImVec2(" << ImGui::GetStyle().TouchExtraPadding.x << "f, "
            << ImGui::GetStyle().TouchExtraPadding.y << "f);\n";
        oss << "style.IndentSpacing = " << ImGui::GetStyle().IndentSpacing << "f;\n";
        oss << "style.ColumnsMinSpacing = " << ImGui::GetStyle().ColumnsMinSpacing << "f;\n";
        oss << "style.ScrollbarSize = " << ImGui::GetStyle().ScrollbarSize << "f;\n";
        oss << "style.ScrollbarRounding = " << ImGui::GetStyle().ScrollbarRounding << "f;\n";
        oss << "style.GrabMinSize = " << ImGui::GetStyle().GrabMinSize << "f;\n";
        oss << "style.GrabRounding = " << ImGui::GetStyle().GrabRounding << "f;\n";
        oss << "style.TabRounding = " << ImGui::GetStyle().TabRounding << "f;\n";
        oss << "style.TabBorderSize = " << ImGui::GetStyle().TabBorderSize << "f;\n";
        oss << "style.TabMinWidthForCloseButton = " << ImGui::GetStyle().TabMinWidthForCloseButton << "f;\n";
        oss << "style.ColorButtonPosition = " << ToString(ImGui::GetStyle().ColorButtonPosition) << ";\n";
        oss << "style.ButtonTextAlign = ImVec2(" << ImGui::GetStyle().ButtonTextAlign.x << "f, "
            << ImGui::GetStyle().ButtonTextAlign.y << "f);\n";
        oss << "style.SelectableTextAlign = ImVec2(" << ImGui::GetStyle().SelectableTextAlign.x << "f, "
            << ImGui::GetStyle().SelectableTextAlign.y << "f);\n";
        oss << "style.Colors[ImGuiCol_Text] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_Text].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Text].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Text].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Text].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TextDisabled] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_TextDisabled].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TextDisabled].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TextDisabled].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TextDisabled].w << "f);\n";
        oss << "style.Colors[ImGuiCol_WindowBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_WindowBg].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_WindowBg].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ChildBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_ChildBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ChildBg].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_ChildBg].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_ChildBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_PopupBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_PopupBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PopupBg].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_PopupBg].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_PopupBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_Border] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_Border].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Border].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Border].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Border].w << "f);\n";
        oss << "style.Colors[ImGuiCol_BorderShadow] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_BorderShadow].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_BorderShadow].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_BorderShadow].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_BorderShadow].w << "f);\n";
        oss << "style.Colors[ImGuiCol_FrameBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_FrameBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_FrameBg].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_FrameBg].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_FrameBgActive] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TitleBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_TitleBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TitleBg].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TitleBg].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TitleBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TitleBgActive] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed].w << "f);\n";
        oss << "style.Colors[ImGuiCol_MenuBarBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabHovered].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabActive].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_CheckMark] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_CheckMark].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_CheckMark].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_CheckMark].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_CheckMark].w << "f);\n";
        oss << "style.Colors[ImGuiCol_SliderGrab] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_SliderGrab].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SliderGrab].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SliderGrab].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SliderGrab].w << "f);\n";
        oss << "style.Colors[ImGuiCol_SliderGrabActive] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_Button] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_Button].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Button].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Button].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Button].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ButtonHovered] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ButtonActive] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_ButtonActive].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_ButtonActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ButtonActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ButtonActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_Header] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_Header].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Header].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Header].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Header].w << "f);\n";
        oss << "style.Colors[ImGuiCol_HeaderHovered] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_HeaderActive] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_HeaderActive].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_HeaderActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_HeaderActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_HeaderActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_Separator] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_Separator].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Separator].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Separator].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Separator].w << "f);\n";
        oss << "style.Colors[ImGuiCol_SeparatorHovered] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_SeparatorActive] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ResizeGrip] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ResizeGripActive] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_Tab] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_Tab].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Tab].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_Tab].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_Tab].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TabHovered] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_TabHovered].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TabActive] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_TabActive].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabActive].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TabActive].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TabActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TabUnfocused] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocused].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocused].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocused].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocused].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocusedActive].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocusedActive].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocusedActive].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TabUnfocusedActive].w << "f);\n";
        oss << "style.Colors[ImGuiCol_PlotLines] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_PlotLines].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotLines].y << "f, " << ImGui::GetStyle().Colors[ImGuiCol_PlotLines].z
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_PlotLines].w << "f);\n";
        oss << "style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_PlotLinesHovered].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotLinesHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotLinesHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotLinesHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_PlotHistogram] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram].w << "f);\n";
        oss << "style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered].w << "f);\n";
        oss << "style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_DragDropTarget] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget].w << "f);\n";
        oss << "style.Colors[ImGuiCol_NavHighlight] = ImVec4(" << ImGui::GetStyle().Colors[ImGuiCol_NavHighlight].x
            << "f, " << ImGui::GetStyle().Colors[ImGuiCol_NavHighlight].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavHighlight].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavHighlight].w << "f);\n";
        oss << "style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingHighlight].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingHighlight].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingHighlight].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingHighlight].w << "f);\n";
        oss << "style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingDimBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingDimBg].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingDimBg].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_NavWindowingDimBg].w << "f);\n";
        oss << "style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4("
            << ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg].x << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg].y << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg].z << "f, "
            << ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg].w << "f);\n";
        return String{oss.str()};
    }
    void ImGuiUIEditor::Render()
    {
        if (!m_IsVisible)
            return;
        ImGui::Begin("UI Editor", &m_IsVisible);
        if (ImGui::Button("Export Style to C++"))
        {
            auto style = ConvertStyleToCpp();
            ImGui::SetClipboardText(style.c_str());
        }
        auto& style = ImGui::GetStyle();
        ImGui::DragFloat("Alpha", &style.Alpha, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("DisabledAlpha", &style.DisabledAlpha, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat2("WindowPadding", &style.WindowPadding.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("WindowRounding", &style.WindowRounding, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("WindowBorderSize", &style.WindowBorderSize, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("WindowMinSize", &style.WindowMinSize.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("WindowTitleAlign", &style.WindowTitleAlign.x, 0.1f, 0.0f, 1.0f);
        if (ImGui::BeginCombo("WindowMenuButtonPosition", ToString(style.WindowMenuButtonPosition).c_str()))
        {
            for (int i = -1; i < ImGuiDir_COUNT; i++)
            {
                bool isSelected = style.WindowMenuButtonPosition == i;
                if (ImGui::Selectable(ToString((ImGuiDir)i).c_str(), isSelected))
                {
                    style.WindowMenuButtonPosition = static_cast<ImGuiDir>(i);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::DragFloat("ChildRounding", &style.ChildRounding, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("ChildBorderSize", &style.ChildBorderSize, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("PopupRounding", &style.PopupRounding, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("PopupBorderSize", &style.PopupBorderSize, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("FramePadding", &style.FramePadding.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("FrameRounding", &style.FrameRounding, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("FrameBorderSize", &style.FrameBorderSize, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("ItemSpacing", &style.ItemSpacing.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("ItemInnerSpacing", &style.ItemInnerSpacing.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("CellPadding", &style.CellPadding.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat2("TouchExtraPadding", &style.TouchExtraPadding.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("IndentSpacing", &style.IndentSpacing, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("ColumnsMinSpacing", &style.ColumnsMinSpacing, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("ScrollbarSize", &style.ScrollbarSize, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("GrabMinSize", &style.GrabMinSize, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("GrabRounding", &style.GrabRounding, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("TabRounding", &style.TabRounding, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("TabBorderSize", &style.TabBorderSize, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("TabMinWidthForCloseButton", &style.TabMinWidthForCloseButton, 0.1f, 0.0f, 100.0f);
        if (ImGui::BeginCombo("ColorButtonPosition", ToString(style.ColorButtonPosition).c_str()))
        {
            for (int i = -1; i < ImGuiDir_COUNT; i++)
            {
                bool isSelected = style.ColorButtonPosition == i;
                if (ImGui::Selectable(ToString((ImGuiDir)i).c_str(), isSelected))
                {
                    style.ColorButtonPosition = static_cast<ImGuiDir>(i);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::DragFloat2("ButtonTextAlign", &style.ButtonTextAlign.x, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat2("SelectableTextAlign", &style.SelectableTextAlign.x, 0.1f, 0.0f, 1.0f);

        ImGui::ColorEdit4("Text", &style.Colors[ImGuiCol_Text].x);
        ImGui::ColorEdit4("TextDisabled", &style.Colors[ImGuiCol_TextDisabled].x);
        ImGui::ColorEdit4("WindowBg", &style.Colors[ImGuiCol_WindowBg].x);
        ImGui::ColorEdit4("ChildBg", &style.Colors[ImGuiCol_ChildBg].x);
        ImGui::ColorEdit4("PopupBg", &style.Colors[ImGuiCol_PopupBg].x);
        ImGui::ColorEdit4("Border", &style.Colors[ImGuiCol_Border].x);
        ImGui::ColorEdit4("BorderShadow", &style.Colors[ImGuiCol_BorderShadow].x);
        ImGui::ColorEdit4("FrameBg", &style.Colors[ImGuiCol_FrameBg].x);
        ImGui::ColorEdit4("FrameBgHovered", &style.Colors[ImGuiCol_FrameBgHovered].x);
        ImGui::ColorEdit4("FrameBgActive", &style.Colors[ImGuiCol_FrameBgActive].x);
        ImGui::ColorEdit4("TitleBg", &style.Colors[ImGuiCol_TitleBg].x);
        ImGui::ColorEdit4("TitleBgActive", &style.Colors[ImGuiCol_TitleBgActive].x);
        ImGui::ColorEdit4("TitleBgCollapsed", &style.Colors[ImGuiCol_TitleBgCollapsed].x);
        ImGui::ColorEdit4("MenuBarBg", &style.Colors[ImGuiCol_MenuBarBg].x);
        ImGui::ColorEdit4("ScrollbarBg", &style.Colors[ImGuiCol_ScrollbarBg].x);
        ImGui::ColorEdit4("ScrollbarGrab", &style.Colors[ImGuiCol_ScrollbarGrab].x);
        ImGui::ColorEdit4("ScrollbarGrabHovered", &style.Colors[ImGuiCol_ScrollbarGrabHovered].x);
        ImGui::ColorEdit4("ScrollbarGrabActive", &style.Colors[ImGuiCol_ScrollbarGrabActive].x);
        ImGui::ColorEdit4("CheckMark", &style.Colors[ImGuiCol_CheckMark].x);
        ImGui::ColorEdit4("SliderGrab", &style.Colors[ImGuiCol_SliderGrab].x);
        ImGui::ColorEdit4("SliderGrabActive", &style.Colors[ImGuiCol_SliderGrabActive].x);
        ImGui::ColorEdit4("Button", &style.Colors[ImGuiCol_Button].x);
        ImGui::ColorEdit4("ButtonHovered", &style.Colors[ImGuiCol_ButtonHovered].x);
        ImGui::ColorEdit4("ButtonActive", &style.Colors[ImGuiCol_ButtonActive].x);
        ImGui::ColorEdit4("Header", &style.Colors[ImGuiCol_Header].x);
        ImGui::ColorEdit4("HeaderHovered", &style.Colors[ImGuiCol_HeaderHovered].x);
        ImGui::ColorEdit4("HeaderActive", &style.Colors[ImGuiCol_HeaderActive].x);
        ImGui::ColorEdit4("Separator", &style.Colors[ImGuiCol_Separator].x);
        ImGui::ColorEdit4("SeparatorHovered", &style.Colors[ImGuiCol_SeparatorHovered].x);
        ImGui::ColorEdit4("SeparatorActive", &style.Colors[ImGuiCol_SeparatorActive].x);
        ImGui::ColorEdit4("ResizeGrip", &style.Colors[ImGuiCol_ResizeGrip].x);
        ImGui::ColorEdit4("ResizeGripHovered", &style.Colors[ImGuiCol_ResizeGripHovered].x);
        ImGui::ColorEdit4("ResizeGripActive", &style.Colors[ImGuiCol_ResizeGripActive].x);
        ImGui::ColorEdit4("Tab", &style.Colors[ImGuiCol_Tab].x);
        ImGui::ColorEdit4("TabHovered", &style.Colors[ImGuiCol_TabHovered].x);
        ImGui::ColorEdit4("TabActive", &style.Colors[ImGuiCol_TabActive].x);
        ImGui::ColorEdit4("TabUnfocused", &style.Colors[ImGuiCol_TabUnfocused].x);
        ImGui::ColorEdit4("TabUnfocusedActive", &style.Colors[ImGuiCol_TabUnfocusedActive].x);
        ImGui::ColorEdit4("PlotLines", &style.Colors[ImGuiCol_PlotLines].x);
        ImGui::ColorEdit4("PlotLinesHovered", &style.Colors[ImGuiCol_PlotLinesHovered].x);
        ImGui::ColorEdit4("PlotHistogram", &style.Colors[ImGuiCol_PlotHistogram].x);
        ImGui::ColorEdit4("PlotHistogramHovered", &style.Colors[ImGuiCol_PlotHistogramHovered].x);
        ImGui::ColorEdit4("TableHeaderBg", &style.Colors[ImGuiCol_TableHeaderBg].x);
        ImGui::ColorEdit4("TableBorderStrong", &style.Colors[ImGuiCol_TableBorderStrong].x);
        ImGui::ColorEdit4("TableBorderLight", &style.Colors[ImGuiCol_TableBorderLight].x);
        ImGui::ColorEdit4("TableRowBg", &style.Colors[ImGuiCol_TableRowBg].x);
        ImGui::ColorEdit4("TableRowBgAlt", &style.Colors[ImGuiCol_TableRowBgAlt].x);
        ImGui::ColorEdit4("TextSelectedBg", &style.Colors[ImGuiCol_TextSelectedBg].x);
        ImGui::ColorEdit4("DragDropTarget", &style.Colors[ImGuiCol_DragDropTarget].x);
        ImGui::ColorEdit4("NavHighlight", &style.Colors[ImGuiCol_NavHighlight].x);
        ImGui::ColorEdit4("NavWindowingHighlight", &style.Colors[ImGuiCol_NavWindowingHighlight].x);
        ImGui::ColorEdit4("NavWindowingDimBg", &style.Colors[ImGuiCol_NavWindowingDimBg].x);
        ImGui::ColorEdit4("ModalWindowDimBg", &style.Colors[ImGuiCol_ModalWindowDimBg].x);
        ImGui::End();
    }
} // namespace BeeEngine
