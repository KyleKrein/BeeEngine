//
// Created by alexl on 26.05.2023.
//

#pragma once
#include "BeeEngine.h"
#include "ConfigFile.h"

namespace BeeEngine::Editor
{
    class EditorApplication : public Application
    {
    public:
        explicit EditorApplication(ConfigFile&& config) noexcept;
        ~EditorApplication() override;
        void AddDebugOverlay();

    private:
        ConfigFile m_Config;
    };
} // namespace BeeEngine::Editor
