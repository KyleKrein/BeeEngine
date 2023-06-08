//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/GraphicsDevice.h"
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanGraphicsDevice: public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(WindowHandler& window);
        ~VulkanGraphicsDevice() override;

        [[nodiscard]] Ref<Surface> GetSurface() const override;
        [[nodiscard]] Ref<CommandPool> GetCommandPool() const override;
        [[nodiscard]] DeviceID GetDeviceID() const override;
        [[nodiscard]] Ref<GraphicsQueue> GetGraphicsQueue() const override;
        [[nodiscard]] Ref<GraphicsQueue> GetPresentQueue() const override;
        [[nodiscard]] Ref<SwapChain> GetSwapChain() const override;

    private:
        VkDevice m_Device;
    };
}
