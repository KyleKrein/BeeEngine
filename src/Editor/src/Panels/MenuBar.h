//
// Created by alexl on 05.06.2023.
//

#pragma once

#include "Gui/ImGui/IImGuiElement.h"

namespace BeeEngine::Editor
{
    struct MenuBarItem
    {
        String Name;
        std::function<void()> Action;
    };
    struct MenuBarElement
    {
        String Name;
        std::vector<MenuBarItem> Children;

        void AddChild(const MenuBarItem& element) { Children.push_back(element); }
    };
    class MenuBar : public BeeEngine::Internal::IImGuiElement
    {
    public:
        MenuBar() = default;
        ~MenuBar() override = default;

        void Render() override;
        void Update() override;
        void OnEvent(EventDispatcher& event) override;

        void AddElement(const MenuBarElement& element);

    private:
        std::vector<MenuBarElement> m_Elements;
    };
} // namespace BeeEngine::Editor
