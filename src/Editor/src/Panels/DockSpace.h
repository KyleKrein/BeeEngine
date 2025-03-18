//
// Created by alexl on 01.06.2023.
//

#pragma once

#include "Gui/ImGui/IImGuiElement.h"

namespace BeeEngine::Editor
{
    class DockSpace
    {
    public:
        void Start(bool showMenubar) noexcept;
        void End() noexcept;
    };
} // namespace BeeEngine::Editor
