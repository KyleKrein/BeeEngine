//
// Created by alexl on 10.06.2023.
//
#define VK_VERSION_
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "ImGuiControllerVulkan.h"

namespace BeeEngine
{

    void BeeEngine::ImGuiControllerVulkan::Initialize(uint16_t width, uint16_t height, uint64_t glfwwindow)
    {
        //return;
        window = reinterpret_cast<GLFWwindow *>(glfwwindow);

        Internal::VulkanGraphicsDevice& graphicsDevice = *(Internal::VulkanGraphicsDevice*)&WindowHandler::GetInstance()->GetGraphicsDevice();

        //1: create descriptor pool for IMGUI
        // the size of the pool is very oversize, but it's copied from imgui demo itself.
        VkDescriptorPoolSize pool_sizes[] =
                {
                        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
                };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;


        vkCreateDescriptorPool(graphicsDevice.GetDevice(), &pool_info, nullptr, &g_DescriptorPool);

        // 2: initialize imgui library

        //this initializes the core structures of imgui
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        SetDefaultTheme();
        //SetDarkThemeColors();

        //this initializes imgui for SDL
        ImGui_ImplGlfw_InitForVulkan(window, true);

        //this initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = ((Internal::VulkanInstance*)&WindowHandler::GetInstance()->GetAPIInstance())->GetHandle();
        init_info.PhysicalDevice = graphicsDevice.GetPhysicalDevice();
        init_info.Device = graphicsDevice.GetDevice();
        init_info.Queue = graphicsDevice.GetGraphicsQueue().GetQueue();
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info, graphicsDevice.GetSwapChain().GetRenderPass());

        auto cmd = graphicsDevice.BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(cmd);
        graphicsDevice.EndSingleTimeCommands(cmd);
        //execute a gpu command to upload imgui font textures
        /*immediate_submit([&](VkCommandBuffer cmd) {
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        });*/

        //clear font textures from cpu data
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        /*//add the destroy the imgui created structures
        _mainDeletionQueue.push_function([=]() {

            vkDestroyDescriptorPool(_device, imguiPool, nullptr);
            ImGui_ImplVulkan_Shutdown();
        });*/
    }

    void ImGuiControllerVulkan::Update()
    {
        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiControllerVulkan::Render()
    {
        //return;
        auto& io = ImGui::GetIO();
        // Rendering
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Internal::VulkanRendererAPI::GetInstance().GetCurrentCommandBuffer());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiControllerVulkan::Shutdown()
    {
        //return;
        // Cleanup
        Internal::VulkanGraphicsDevice& graphicsDevice = *(Internal::VulkanGraphicsDevice*)&WindowHandler::GetInstance()->GetGraphicsDevice();
        vkDestroyDescriptorPool(graphicsDevice.GetDevice(), g_DescriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        /*auto err = vkDeviceWaitIdle(g_Device);
        check_vk_result(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
    */}

    bool ImGuiControllerVulkan::IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties,
                                                     const char *extension)
    {
        for (const VkExtensionProperties& p : properties)
            if (strcmp(p.extensionName, extension) == 0)
                return true;
        return false;
    }

    void ImGuiControllerVulkan::SetupVulkan(ImVector<const char *> instance_extensions)
    {
        VkResult err;
        Internal::VulkanGraphicsDevice& graphicsDevice = *(Internal::VulkanGraphicsDevice*)&WindowHandler::GetInstance()->GetGraphicsDevice();
        g_Device = graphicsDevice.GetDevice();
        g_PhysicalDevice = graphicsDevice.GetPhysicalDevice();
        g_QueueFamily = graphicsDevice.GetQueueFamilyIndices().GraphicsFamily.value();
        g_Queue = graphicsDevice.GetGraphicsQueue().GetQueue();
        g_Instance = ((Internal::VulkanInstance*)&WindowHandler::GetInstance()->GetAPIInstance())->GetHandle();

        // Create Descriptor Pool
        {
            VkDescriptorPoolSize pool_sizes[] =
                    {
                            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
                    };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
            pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;
            err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
            check_vk_result(err);
        }
    }

    void ImGuiControllerVulkan::FramePresent(ImGui_ImplVulkanH_Window *wd)
    {
        if (g_SwapChainRebuild)
            return;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &render_complete_semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd->Swapchain;
        info.pImageIndices = &wd->FrameIndex;
        VkResult err = vkQueuePresentKHR(g_Queue, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
        {
            g_SwapChainRebuild = true;
            return;
        }
        check_vk_result(err);
        wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
    }

    void
    ImGuiControllerVulkan::SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height)
    {
        wd->Surface = surface;

        // Check for WSI support
        VkBool32 res;
        vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
        if (res != VK_TRUE)
        {
            fprintf(stderr, "Error no WSI support on physical device 0\n");
            exit(-1);
        }

        // Select Surface Format
        const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

        // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
        VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
        VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
        wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
        //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

        // Create SwapChain, RenderPass, Framebuffer, etc.
        IM_ASSERT(g_MinImageCount >= 2);
        ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
    }

    void ImGuiControllerVulkan::FrameRender(VkCommandBuffer commandBuffer, ImDrawData *draw_data)
    {
        ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);
    }
}