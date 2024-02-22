//
// Created by Александр Лебедев on 25.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"
#include "vk_mem_alloc.h"
#include "VulkanGraphicsDevice.h"
#include "Core/DeletionQueue.h"

namespace BeeEngine::Internal
{
    struct VulkanBuffer
    {
        vk::Buffer Buffer;
        VmaAllocation Memory {VK_NULL_HANDLE};
        //VulkanBuffer() = default;

        /*VulkanBuffer(VulkanBuffer&& other) noexcept
        {
            Buffer = other.Buffer;
            Memory = other.Memory;
            other.Buffer = VK_NULL_HANDLE;
            other.Memory = VK_NULL_HANDLE;
        }

        VulkanBuffer& operator=(VulkanBuffer&& other) noexcept
        {
            Buffer = other.Buffer;
            Memory = other.Memory;
            other.Buffer = VK_NULL_HANDLE;
            other.Memory = VK_NULL_HANDLE;
            return *this;
        }

        VulkanBuffer(const VulkanBuffer& other) = delete;
        VulkanBuffer& operator=(const VulkanBuffer& other) = delete;

        ~VulkanBuffer()
        {
            if(Buffer == VK_NULL_HANDLE || Memory == VK_NULL_HANDLE)
                return;
            DeletionQueue::Main().PushFunction([=, this]() {
                vmaDestroyBuffer(GetVulkanAllocator(), Buffer, Memory);
            });
        }*/
    };
}
