//
// Created by alexl on 10.06.2023.
//
#include "Renderer/Renderer.h"
#if defined(BEE_COMPILE_VULKAN)
#define VK_VERSION_
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "backends/imgui_impl_vulkan.h"
#if defined(BEE_COMPILE_SDL)
#include "backends/imgui_impl_sdl3.h"
#endif
#if defined(WINDOWS)
#include "backends/imgui_impl_win32.h"
#endif
#include "ImGuiControllerVulkan.h"

namespace BeeEngine::Internal
{
    std::function<void()> ImGuiControllerVulkan::s_ShutdownFunction = nullptr;
    void ImGuiControllerVulkan::Initialize(uint16_t width, uint16_t height, uintptr_t window)
    {
        m_Window = (void*)window;
        Internal::VulkanGraphicsDevice& graphicsDevice = VulkanGraphicsDevice::GetInstance();

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
        //TODO: fix multiviewport and uncomment //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
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

        SetupFunctionsForBackend();
        //this initializes imgui for Backend
        switch (WindowHandler::GetAPI())
        {
            case WindowHandlerAPI::SDL:
#if defined(BEE_COMPILE_SDL)
                ImGui_ImplSDL3_InitForVulkan((SDL_Window*)m_Window);
#endif
                break;
            case WindowHandlerAPI::WinAPI:
#if defined(WINDOWS)
                ImGui_ImplWin32_Init(m_Window);
#endif
                break;
        }
        //this initializes imgui for Vulkan

        auto& swapchain = graphicsDevice.GetSwapChain();
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = ((Internal::VulkanInstance*)&WindowHandler::GetInstance()->GetAPIInstance())->GetHandle();
        init_info.PhysicalDevice = graphicsDevice.GetPhysicalDevice();
        init_info.Device = graphicsDevice.GetDevice();
        init_info.QueueFamily = graphicsDevice.GetQueueFamilyIndices().GraphicsFamily.value();
        init_info.Queue = graphicsDevice.GetGraphicsQueue();
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.RenderPass = nullptr;
        init_info.Subpass = 0;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = check_vk_result;
        init_info.UseDynamicRendering = true;
        VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {};
        pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        pipelineRenderingCreateInfo.colorAttachmentCount = 1;
        pipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
        VkFormat colorAttachmentFormats[] = { static_cast<VkFormat>(swapchain.GetFormat()) };
        pipelineRenderingCreateInfo.pColorAttachmentFormats = colorAttachmentFormats;
        pipelineRenderingCreateInfo.pNext = nullptr;
        pipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
        pipelineRenderingCreateInfo.viewMask = 0;

        init_info.PipelineRenderingCreateInfo = pipelineRenderingCreateInfo;

        ImGui_ImplVulkan_Init(&init_info);
    }

    void ImGuiControllerVulkan::Update()
    {
        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        m_NewFrameBackend();
        ImGui::NewFrame();
    }

    void ImGuiControllerVulkan::Render(CommandBuffer& commandBuffer)
    {
        //return;
        auto& io = ImGui::GetIO();
        // Rendering
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetBufferHandleAs<vk::CommandBuffer>());
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
        //Internal::VulkanGraphicsDevice& graphicsDevice = *(Internal::VulkanGraphicsDevice*)&WindowHandler::GetInstance()->GetGraphicsDevice();
        //vkDestroyDescriptorPool(graphicsDevice.GetDevice(), g_DescriptorPool, nullptr);
        //ImGui_ImplVulkan_Shutdown();
        auto& device = Internal::VulkanGraphicsDevice::GetInstance();
        auto err = vkDeviceWaitIdle(device.GetDevice());
        check_vk_result(err);
        //ImGui::DestroyContext();

        s_ShutdownFunction = [device = device.GetDevice(), pool = g_DescriptorPool]
        {
            BeeCoreTrace("Imgui Vulkan Shutdown");
            ImGui_ImplVulkan_Shutdown();
            switch(WindowHandler::GetAPI())
            {
                case WindowHandlerAPI::SDL:
#if defined(BEE_COMPILE_SDL)
                    ImGui_ImplSDL3_Shutdown();
#endif
                    break;
                case WindowHandlerAPI::WinAPI:
#if defined(WINDOWS)
                    ImGui_ImplWin32_Shutdown();
#endif
                    break;
            }
            vkDestroyDescriptorPool(device, pool, nullptr);
        };
    }

    void ImGuiControllerVulkan::SetupFunctionsForBackend()
    {
        switch (WindowHandler::GetAPI()) {
            case WindowHandlerAPI::SDL:
#if defined(BEE_COMPILE_SDL)
                m_NewFrameBackend = ImGui_ImplSDL3_NewFrame;
#endif
                break;
            case WindowHandlerAPI::WinAPI:
#if defined(WINDOWS)
                m_NewFrameBackend = ImGui_ImplWin32_NewFrame;
#endif
                break;
        }

    }
}
#endif