//
// Created by Aleksandr on 10.03.2024.
//

#pragma once
#include "Core/CodeSafety/Expects.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/RenderingQueue.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanImage.h"
#include <vector>

namespace BeeEngine::Internal
{

    class VulkanFrameBuffer final : public FrameBuffer
    {
    public:
        VulkanFrameBuffer(const FrameBufferPreferences& preferences);
        ~VulkanFrameBuffer() override;

        CommandBuffer Bind() override;

        void Unbind(CommandBuffer& commandBuffer) override;

        void Resize(uint32_t width, uint32_t height) override;

        void Invalidate() override;

        [[nodiscard]] uintptr_t GetColorAttachmentImGuiRendererID(uint32_t index) const override;

        [[nodiscard]] uintptr_t GetDepthAttachmentImGuiRendererID() const override;

        [[nodiscard]] GPUTextureResource& GetColorAttachmentResource(size_t index) override
        {
            BeeExpects(index < m_ColorAttachmentsTextures.size());
            return m_ColorAttachmentsTextures[index];
        }

        [[nodiscard]] BindingSet& GetColorBindingSet() override { return *m_ColorBindingSet; }

        [[nodiscard]] int ReadPixel(uint32_t attachmentIndex, int x, int y) const override;

        VulkanImage GetColorAttachment(uint32_t index) const;

        [[nodiscard]] DumpedImage DumpAttachment(uint32_t attachmentIndex) const override;

    private:
        void CreateImageAndImageView(VulkanImage& image,
                                     vk::ImageView& view,
                                     FrameBufferTextureFormat format,
                                     FrameBufferTextureUsage usage);

    private:
        RenderingQueue m_RenderingQueue;
        std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecification;
        FrameBufferTextureSpecification m_DepthAttachmentSpecification;
        FrameBufferPreferences m_Preferences;
        bool m_Initiated{false};

        std::vector<VulkanGPUTextureResource> m_ColorAttachmentsTextures;
        struct AttachmentReadBuffer
        {
            VulkanBuffer Buffer;
            bool IsUpdated{false};
        };
        mutable std::vector<AttachmentReadBuffer> m_ColorAttachmentsReadBuffers;
        void ResetReadBuffers()
        {
            for (auto& buffer : m_ColorAttachmentsReadBuffers)
            {
                buffer.IsUpdated = false;
            }
        }
        void CopyToBufferIfNotCopied(uint32_t index) const;
        Scope<VulkanGPUTextureResource> m_DepthAttachmentTexture{nullptr};
        Ref<BindingSet> m_ColorBindingSet{nullptr};

        bool m_Invalid{true};

        VulkanGraphicsDevice& m_GraphicsDevice;
        vk::CommandBuffer m_CurrentCommandBuffer;
    };

} // namespace BeeEngine::Internal
