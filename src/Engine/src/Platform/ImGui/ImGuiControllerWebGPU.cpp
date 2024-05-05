//
// Created by Александр Лебедев on 01.07.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "ImGuiControllerWebGPU.h"
#include "Renderer/Renderer.h"
#include "backends/imgui_impl_wgpu.h"
#if defined(BEE_COMPILE_SDL)
#include "backends/imgui_impl_sdl3.h"
#endif
#include "Core/Application.h"

namespace BeeEngine::Internal
{

    void ImGuiControllerWebGPU::Initialize(uint16_t width, uint16_t height, uintptr_t window)
    {
        // this initializes the core structures of imgui
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
        // ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        SetDefaultTheme();

        switch (Application::GetOsPlatform())
        {
#if defined(BEE_COMPILE_SDL)
            case OSPlatform::Linux:
                ImGui_ImplSDL3_InitForVulkan((SDL_Window*)window);
                break;
            case OSPlatform::Mac:
                ImGui_ImplSDL3_InitForMetal((SDL_Window*)window);
                break;
            case OSPlatform::Windows:
                ImGui_ImplSDL3_InitForVulkan((SDL_Window*)window);
                break;
#endif

            case OSPlatform::iOS:
            case OSPlatform::Android:
            case OSPlatform::None:
            default:
                BeeCoreError("ImGuiControllerWebGPU::Initialize: Platform {} is not supported",
                             ToString(Application::GetOsPlatform()));
                break;
        }
        ImGui_ImplWGPU_InitInfo init_info = {};
        init_info.Device = m_Device.GetDevice();
        init_info.DepthStencilFormat = m_Device.GetSwapChain().GetDepthFormat();
        init_info.RenderTargetFormat = m_Device.GetSwapChain().GetFormat();
        init_info.NumFramesInFlight = 2;
        ImGui_ImplWGPU_Init(&init_info);
        ImGui_ImplWGPU_CreateDeviceObjects();
    }

    void ImGuiControllerWebGPU::Update()
    {
        ImGui_ImplWGPU_NewFrame();
#if defined(BEE_COMPILE_SDL)
        ImGui_ImplSDL3_NewFrame();
#endif
        ImGui::NewFrame();
    }

    void ImGuiControllerWebGPU::Render()
    {
        auto& io = ImGui::GetIO();
        ImGui::Render();
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(),
                                      (WGPURenderPassEncoder)Renderer::GetCurrentRenderPass().GetHandle());
        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiControllerWebGPU::Shutdown()
    {
        ImGui_ImplWGPU_Shutdown();
#if defined(BEE_COMPILE_SDL)
        ImGui_ImplSDL3_Shutdown();
#endif
        // ImGui::DestroyContext();
    }

    ImGuiControllerWebGPU::~ImGuiControllerWebGPU() {}
} // namespace BeeEngine::Internal
#endif