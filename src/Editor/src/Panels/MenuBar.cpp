//
// Created by alexl on 05.06.2023.
//

#include "MenuBar.h"

namespace BeeEngine::Editor
{

    void MenuBar::Render()
    {
        if (ImGui::BeginMenuBar())
        {
            for (auto& MenuElement : m_Elements)
            {
                if (ImGui::BeginMenu(MenuElement.Name.c_str()))
                {
                    for (auto& ChildElement : MenuElement.Children)
                    {
                        if (ImGui::MenuItem(ChildElement.Name.c_str()))
                        {
                            ChildElement.Action();
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenuBar();
        }
    }

    void MenuBar::Update() {}

    void MenuBar::AddElement(const MenuBarElement& element)
    {
        m_Elements.push_back(element);
    }

    void MenuBar::OnEvent(EventDispatcher& event) {}
} // namespace BeeEngine::Editor
