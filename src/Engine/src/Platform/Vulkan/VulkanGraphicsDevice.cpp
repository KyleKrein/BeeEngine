//
// Created by alexl on 09.06.2023.
//

#include "VulkanGraphicsDevice.h"


namespace BeeEngine::Internal
{

    VulkanGraphicsDevice::VulkanGraphicsDevice(WindowHandler &window)
    {

    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {

    }

    Ref<Surface> VulkanGraphicsDevice::GetSurface() const
    {
        return BeeEngine::Ref<Surface>();
    }

    Ref<CommandPool> VulkanGraphicsDevice::GetCommandPool() const
    {
        return BeeEngine::Ref<CommandPool>();
    }

    DeviceID VulkanGraphicsDevice::GetDeviceID() const
    {
        return DeviceID();
    }

    Ref<GraphicsQueue> VulkanGraphicsDevice::GetGraphicsQueue() const
    {
        return BeeEngine::Ref<GraphicsQueue>();
    }

    Ref<GraphicsQueue> VulkanGraphicsDevice::GetPresentQueue() const
    {
        return BeeEngine::Ref<GraphicsQueue>();
    }

    Ref<SwapChain> VulkanGraphicsDevice::GetSwapChain() const
    {
        return BeeEngine::Ref<SwapChain>();
    }
}
