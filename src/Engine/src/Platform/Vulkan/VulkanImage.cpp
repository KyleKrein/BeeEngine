//
// Created by Александр Лебедев on 29.06.2023.
//
#include "VulkanImage.h"
#include "Core/DeletionQueue.h"
#include "VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{
    void VulkanImage::Destroy()
    {
        DeletionQueue::Frame().PushFunction([=, this]()
                                            {
                                                vmaDestroyImage(GetVulkanAllocator(), Image, Memory);
                                            });
    }
}