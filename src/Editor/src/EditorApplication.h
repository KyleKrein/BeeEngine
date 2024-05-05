//
// Created by alexl on 26.05.2023.
//

#pragma once
#include "BeeEngine.h"

namespace BeeEngine::Editor
{
    class EditorApplication : public Application
    {
    public:
        explicit EditorApplication(ApplicationProperties&& properties) noexcept;
    };
} // namespace BeeEngine::Editor
