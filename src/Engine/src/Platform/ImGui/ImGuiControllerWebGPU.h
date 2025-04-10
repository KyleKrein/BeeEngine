//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once
#if defined(BEE_COMPILE_WEBGPU)
#include "ImGuiController.h"
#include "Platform/WebGPU/WebGPUGraphicsDevice.h"

namespace BeeEngine::Internal
{
    class ImGuiControllerWebGPU : public ImGuiController
    {
    public:
        ImGuiControllerWebGPU() : m_Device(WebGPUGraphicsDevice::GetInstance()) {}
        void Initialize(uint16_t width, uint16_t height, uintptr_t window) override;
        void Update() override;
        void Render() override;
        void Shutdown() override;
        ~ImGuiControllerWebGPU() override;

    private:
        WebGPUGraphicsDevice& m_Device;
    };
} // namespace BeeEngine::Internal
#endif