//
// Created by alexl on 10.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#pragma once

#include "ImGuiController.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_sdl3.h"
#include <functional>


namespace BeeEngine::Internal
{
    class ImGuiControllerVulkan: public ImGuiController
    {
        SDL_Window* m_SdlWindow;
    public:
        static std::function<void()> s_ShutdownFunction;
        void Initialize(uint16_t width, uint16_t height, uintptr_t windowHandle) override;
        void Update() override;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        void Render() override;
        void Shutdown() override;
    private:
        VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;
        static void check_vk_result(VkResult err)
        {
            if (err == 0)
                return;
            fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
            if (err < 0)
                abort();
        }
    };
}
#endif