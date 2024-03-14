//
// Created by alexl on 10.06.2023.
//
#pragma once
#if defined(BEE_COMPILE_VULKAN)

#include "ImGuiController.h"
#include <functional>
#include <imgui.h>
#include <vulkan/vulkan.hpp>


namespace BeeEngine::Internal
{
    class ImGuiControllerVulkan: public ImGuiController
    {
        void* m_Window = nullptr;
    public:
        static std::function<void()> s_ShutdownFunction;
        void Initialize(uint16_t width, uint16_t height, uintptr_t windowHandle) override;
        void Update() override;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        void Render() override;
        void Shutdown() override;
    private:
        void SetupFunctionsForBackend();

        void(*m_NewFrameBackend)() = nullptr;
        VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
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