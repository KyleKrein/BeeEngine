//
// Created by Aleksandr on 10.03.2024.
//

#pragma once
#include "VulkanGraphicsDevice.h"
#include "VulkanImage.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/RenderingQueue.h"

namespace BeeEngine::Internal
{

    class VulkanFrameBuffer final: public FrameBuffer
    {
    public:
        VulkanFrameBuffer(const FrameBufferPreferences &preferences);
        ~VulkanFrameBuffer() override;

        CommandBuffer Bind() override;

        void Unbind(CommandBuffer& commandBuffer) override;

        void Resize(uint32_t width, uint32_t height) override;

        void Invalidate() override;

        [[nodiscard]] uintptr_t GetColorAttachmentRendererID(uint32_t index) const override;

        [[nodiscard]] uintptr_t GetDepthAttachmentRendererID() const override;

        [[nodiscard]] int ReadPixel(uint32_t attachmentIndex, int x, int y) const override;
    private:
        void CreateImageAndImageView(VulkanImage& image, vk::ImageView& view, FrameBufferTextureFormat format);
    private:
        RenderingQueue m_RenderingQueue;
        std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecification;
        FrameBufferTextureSpecification m_DepthAttachmentSpecification;
        FrameBufferPreferences m_Preferences;
        bool m_Initiated {false};

        std::vector<VulkanImage> m_ColorAttachmentsTextures;
        std::vector<vk::ImageView> m_ColorAttachmentsTextureViews;
        VulkanImage m_DepthAttachmentTexture {nullptr};
        vk::ImageView m_DepthAttachmentTextureView {nullptr};

        bool m_Invalid {true};

        VulkanGraphicsDevice& m_GraphicsDevice;
        vk::CommandBuffer m_CurrentCommandBuffer;

        std::vector<vk::DescriptorSet> m_ColorDescriptorSets;
        vk::DescriptorSet m_DepthDescriptorSet;

        std::vector<vk::Sampler> m_ColorSamplers;
        vk::Sampler m_DepthSampler;
    };

}
