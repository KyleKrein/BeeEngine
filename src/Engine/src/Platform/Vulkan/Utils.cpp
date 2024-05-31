//
// Created by Александр Лебедев on 26.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#include "Utils.h"

namespace BeeEngine::Internal
{

    vk::ImageCreateInfo
    VulkanInitializer::ImageCreateInfo(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent3D extent)
    {
        vk::ImageCreateInfo info = {};
        info.sType = vk::StructureType::eImageCreateInfo;
        info.pNext = nullptr;

        info.imageType = vk::ImageType::e2D;

        info.format = format;
        info.extent = extent;

        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = vk::SampleCountFlagBits::e1;
        info.tiling = vk::ImageTiling::eOptimal;
        info.usage = usageFlags;

        return info;
    }

    vk::ImageViewCreateInfo
    VulkanInitializer::ImageViewCreateInfo(vk::Format format, vk::Image image, vk::ImageAspectFlags aspectFlags)
    {
        // build a image-view for the depth image to use for rendering
        vk::ImageViewCreateInfo info = {};
        info.sType = vk::StructureType::eImageViewCreateInfo;
        info.pNext = nullptr;

        info.viewType = vk::ImageViewType::e2D;
        info.image = image;
        info.format = format;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.aspectMask = aspectFlags;

        return info;
    }

    vk::PipelineDepthStencilStateCreateInfo
    VulkanInitializer::DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, vk::CompareOp compareOp)
    {
        vk::PipelineDepthStencilStateCreateInfo info = {};
        info.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
        info.pNext = nullptr;

        info.depthTestEnable = bDepthTest ? vk::True : vk::False;
        info.depthWriteEnable = bDepthWrite ? vk::True : vk::False;
        info.depthCompareOp = bDepthTest ? compareOp : vk::CompareOp::eAlways;
        info.depthBoundsTestEnable = vk::False;
        info.minDepthBounds = 0.0f; // Optional
        info.maxDepthBounds = 1.0f; // Optional
        info.stencilTestEnable = vk::False;

        return info;
    }

    vk::ImageSubresourceRange VulkanInitializer::ImageSubresourceRange(vk::ImageAspectFlags aspectFlags)
    {
        vk::ImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = aspectFlags;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = vk::RemainingMipLevels;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = vk::RemainingArrayLayers;
        return subresourceRange;
    }

    vk::SemaphoreSubmitInfo VulkanInitializer::SemaphoreSubmitInfo(vk::Semaphore semaphore,
                                                                   vk::PipelineStageFlags2 stageFlags)
    {
        vk::SemaphoreSubmitInfo info = {};
        info.semaphore = semaphore;
        info.stageMask = stageFlags;
        info.deviceIndex = 0;
        info.value = 1;
        return info;
    }

    vk::CommandBufferSubmitInfo VulkanInitializer::CommandBufferSubmitInfo(vk::CommandBuffer commandBuffer)
    {
        vk::CommandBufferSubmitInfo info = {};
        info.commandBuffer = commandBuffer;
        return info;
    }
} // namespace BeeEngine::Internal
#endif