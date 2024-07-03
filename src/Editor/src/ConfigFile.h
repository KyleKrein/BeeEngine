//
// Created by alexl on 26.05.2023.
//

#pragma once
#include "BeeEngine.h"
#include "Windowing/VSync.h"
#include <Locale/Locale.h>
#include <cstdint>

namespace BeeEngine::Editor
{
    struct ConfigFile
    {
        uint16_t Width = 1280, Height = 720;
        uint16_t X = 0, Y = 0;
        Locale::Localization Locale = Locale::GetSystemLocale();
        float FontSize = 18.0f;
        float ThumbnailSize = 64.0f;
        VSync VSYNC = VSync::On;
        bool IsMaximized = false;
        void Save(const Path& path) const;
        static ConfigFile Load(const Path& path);
        [[nodiscard]] ApplicationProperties GetApplicationProperties() noexcept;
        static std::filesystem::path LoadCompilerConfiguration() noexcept;
        static inline const Path DefaultPath = "config.yaml";
    };
} // namespace BeeEngine::Editor
