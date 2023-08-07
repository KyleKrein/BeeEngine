#pragma once

#include <cstdint>

namespace BeeEngine
{
    class ImGuiController
    {
    public:
        virtual void Initialize(uint16_t width, uint16_t height, uint64_t window) = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
        virtual ~ImGuiController() = default;
        void SetDarkThemeColors();
    protected:
        void SetDefaultTheme(float fontSize = 18.0f);
    };
}