//
// Created by alexl on 10.06.2023.
//

#pragma once

#include "ImGuiController.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_sdl3.h"


namespace BeeEngine
{
    class ImGuiControllerVulkan: public ImGuiController
    {
        GLFWwindow* m_GlfwWindow;
        SDL_Window* m_SdlWindow;
        WindowHandlerAPI windowHandlerAPI;
        int w, h;
        ImGui_ImplVulkanH_Window* wd;
    public:
        void Initialize(uint16_t width, uint16_t height, uint64_t glfwwindow) override;
        void Update() override;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        void Render() override;
        void Shutdown() override;
    private:
        // Data
        VkAllocationCallbacks*   g_Allocator = nullptr;
        VkInstance               g_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice                 g_Device = VK_NULL_HANDLE;
        uint32_t                 g_QueueFamily = (uint32_t)-1;
        VkQueue                  g_Queue = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
        VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

        ImGui_ImplVulkanH_Window g_MainWindowData;
        int                      g_MinImageCount = 2;
        bool                     g_SwapChainRebuild = false;

        static void glfw_error_callback(int error, const char* description)
        {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        }
        static void check_vk_result(VkResult err)
        {
            if (err == 0)
                return;
            fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
            if (err < 0)
                abort();
        }

#ifdef IMGUI_VULKAN_DEBUG_REPORT
        VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
    fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

        bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension);

        void SetupVulkan(ImVector<const char*> instance_extensions);
        void FramePresent(ImGui_ImplVulkanH_Window* wd);

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
        void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);

        void FrameRender(VkCommandBuffer commandBuffer, ImDrawData *draw_data);
    };
}
