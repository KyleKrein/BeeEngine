//
// Created by alexl on 10.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanSwapChain.h"
#include "VulkanGraphicsQueue.h"
#include "VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{
    class VulkanRendererAPI
    {
    public:
        VulkanRendererAPI();

        void Render();
    private:
        VulkanGraphicsDevice& m_GraphicsDevice;
        vk::Device m_Device;
        //Sync
        VulkanFence m_InFlightFence;
        VulkanSemaphore m_ImageAvailableSemaphore;
        VulkanSemaphore m_RenderFinishedSemaphore;
    };
}
