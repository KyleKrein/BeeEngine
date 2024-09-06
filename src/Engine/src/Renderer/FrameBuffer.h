//
// Created by alexl on 14.05.2023.
//

#pragma once
#include "Core/Color4.h"
#include "Core/Coroutines/Task.h"
#include "Core/TypeDefines.h"
#include "Texture.h"

namespace BeeEngine
{
    class CommandBuffer;
}

namespace BeeEngine
{
    /**
     * @enum FrameBufferTextureFormat
     * @brief Specifies the format of textures used in a framebuffer.
     */
    enum class FrameBufferTextureFormat
    {
        None = 0, ///< No format specified.
        // Colors
        RGBA8,      ///< 8-bit RGBA color format.
        RGBA16F,    ///< 16-bit floating point RGBA color format.
        RedInteger, ///< Single-channel integer format.

        // Depth Stencil
        Depth24, ///< 24-bit depth format.

        Depth = Depth24 ///< Default depth format.
    };

    /**
     * @enum FrameBufferTextureUsage
     * @brief Specifies the usage of framebuffer textures.
     */
    enum class FrameBufferTextureUsage
    {
        None = 0,     ///< No usage specified.
        GPUOnly = 1,  ///< Texture used only on the GPU.
        CPUAndGPU = 2 ///< Texture accessible by both CPU and GPU.
    };

    /**
     * @struct FrameBufferTextureSpecification
     * @brief Defines the specification for a framebuffer texture.
     */
    struct FrameBufferTextureSpecification
    {
        FrameBufferTextureSpecification() = default;
        FrameBufferTextureSpecification(FrameBufferTextureFormat format) : TextureFormat(format) {}

        FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None; ///< Format of the texture.
        Color4 ClearColor = Color4::CornflowerBlue;                              ///< Clear color used for this texture.
        float ClearDepth = 1.0f;                                                 ///< Clear depth value.
        int32_t ClearRedInteger = -1; ///< Clear value for red integer textures.
        FrameBufferTextureUsage TextureUsage = FrameBufferTextureUsage::GPUOnly; ///< Texture usage mode.
    };

    /**
     * @struct FrameBufferAttachmentSpecification
     * @brief Specifies the attachments for a framebuffer.
     */
    struct FrameBufferAttachmentSpecification
    {
        FrameBufferAttachmentSpecification() = default;
        FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
            : Attachments(attachments)
        {
        }

        std::vector<FrameBufferTextureSpecification> Attachments; ///< List of texture specifications.
    };

    /**
     * @struct FrameBufferPreferences
     * @brief Specifies the preferences for creating a framebuffer.
     */
    struct FrameBufferPreferences
    {
        uint32_t Width;                                 ///< Width of the framebuffer.
        uint32_t Height;                                ///< Height of the framebuffer.
        uint32_t Samples = 1;                           ///< Number of samples for multisampling.
        bool SwapChainTarget = false;                   ///< Indicates if the framebuffer is a swap chain target.
        FrameBufferAttachmentSpecification Attachments; ///< Attachment specifications.

        FrameBufferPreferences() : Width(1200), Height(720), Samples(1), SwapChainTarget(false) {}
        FrameBufferPreferences(uint32_t width, uint32_t height, uint32_t samples = 1, bool swapChainTarget = false)
            : Width(width), Height(height), Samples(samples), SwapChainTarget(swapChainTarget)
        {
        }
    };

    /**
     * @class DumpedImage
     * @brief Represents an image that has been dumped from a framebuffer attachment.
     */
    class DumpedImage final
    {
    public:
        /**
         * @brief Constructs a dumped image from raw data.
         * @param data Pointer to the image data.
         * @param width Width of the image.
         * @param height Height of the image.
         * @param sizeOfPixel Size of each pixel in bytes.
         * @param format Format of the image.
         */
        DumpedImage(void* data, uint32_t width, uint32_t height, uint32_t sizeOfPixel, FrameBufferTextureFormat format)
        {
            m_Data = malloc(width * height * sizeOfPixel);
            memcpy(m_Data, data, width * height * sizeOfPixel);
        }

        // Deleted copy constructor and assignment operator
        DumpedImage(const DumpedImage& other) = delete;
        DumpedImage& operator=(const DumpedImage& other) = delete;

        DumpedImage(DumpedImage&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_SizeOfPixel = other.m_SizeOfPixel;
            m_Format = other.m_Format;
            other.m_Data = nullptr;
        }
        DumpedImage& operator=(DumpedImage&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_SizeOfPixel = other.m_SizeOfPixel;
            m_Format = other.m_Format;
            other.m_Data = nullptr;
            return *this;
        }
        ~DumpedImage()
        {
            if (m_Data)
            {
                free(m_Data);
            }
        }

        /**
         * @brief Gets the raw image data.
         * @return Pointer to the image data.
         */
        [[nodiscard]] void* GetData() const { return m_Data; }

        /**
         * @brief Gets the width of the image.
         * @return Width of the image.
         */
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }

        /**
         * @brief Gets the height of the image.
         * @return Height of the image.
         */
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }

        /**
         * @brief Gets the size of each pixel in bytes.
         * @return Size of a pixel.
         */
        [[nodiscard]] uint32_t GetSizeOfPixel() const { return m_SizeOfPixel; }

        /**
         * @brief Gets the format of the image.
         * @return Image format.
         */
        [[nodiscard]] FrameBufferTextureFormat GetFormat() const { return m_Format; }

    private:
        void* m_Data = nullptr;            ///< Pointer to the image data.
        uint32_t m_Width;                  ///< Width of the image.
        uint32_t m_Height;                 ///< Height of the image.
        uint32_t m_SizeOfPixel;            ///< Size of each pixel in bytes.
        FrameBufferTextureFormat m_Format; ///< Format of the image.
    };

    /**
     * @class FrameBuffer
     * @brief Abstract base class for framebuffers.
     */
    class FrameBuffer
    {
    public:
        FrameBuffer() = default;
        virtual ~FrameBuffer() = default;

        /**
         * @brief Binds the framebuffer and returns the associated command buffer.
         * Don't forget to unbind the framebuffer when you're done.
         * @return The command buffer used for rendering.
         */
        [[nodiscard]] virtual CommandBuffer Bind() = 0;

        /**
         * @brief Unbinds the framebuffer.
         * @param commandBuffer The command buffer to unbind.
         */
        virtual void Unbind(CommandBuffer& commandBuffer) = 0;

        /**
         * @brief Resizes the framebuffer.
         * @param width New width of the framebuffer.
         * @param height New height of the framebuffer.
         */
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        /**
         * @brief Invalidates the framebuffer, forcing a reallocation of resources.
         */
        virtual void Invalidate() = 0;

        /**
         * @brief Gets the ImGui renderer ID for a color attachment.
         * @param index The index of the color attachment.
         * @return The ImGui renderer ID.
         */
        [[nodiscard]] virtual uintptr_t GetColorAttachmentImGuiRendererID(uint32_t index) const = 0;

        /**
         * @brief Gets the ImGui renderer ID for the depth attachment.
         * @return The ImGui renderer ID for the depth attachment.
         */
        [[nodiscard]] virtual uintptr_t GetDepthAttachmentImGuiRendererID() const = 0;

        /**
         * @brief Gets the GPU texture resource for a color attachment.
         * @param index The index of the color attachment.
         * @return The GPU texture resource.
         */
        [[nodiscard]] virtual GPUTextureResource& GetColorAttachmentResource(size_t index) = 0;

        /**
         * @brief Gets the binding set associated with the color attachments.
         * @return The binding set.
         */
        [[nodiscard]] virtual BindingSet& GetColorBindingSet() = 0;

        /**
         * @brief Reads a pixel value from a color attachment at a specific position.
         * @param attachmentIndex Index of the color attachment. Attachment must be created with CPU and GPU usage
         * @param x X-coordinate of the pixel.
         * @param y Y-coordinate of the pixel.
         * @return The value of the pixel.
         */
        [[nodiscard]] virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) const = 0;

        /**
         * @brief Dumps an image from a color attachment.
         * @param attachmentIndex Index of the color attachment. Attachment must be created with CPU and GPU usage
         * @return A DumpedImage containing the pixel data.
         */
        [[nodiscard]] virtual DumpedImage DumpAttachment(uint32_t attachmentIndex) const = 0;

        /**
         * @brief Creates a framebuffer with the specified preferences.
         * @param preferences Preferences for the framebuffer.
         * @return A scoped pointer to the created framebuffer.
         */
        static Scope<FrameBuffer> Create(const FrameBufferPreferences& preferences);

        /**
         * @brief Creates a framebuffer with the specified preferences (move semantics).
         * @param preferences Preferences for the framebuffer (moved).
         * @return A scoped pointer to the created framebuffer.
         */
        static Scope<FrameBuffer> Create(FrameBufferPreferences&& preferences);

    protected:
        /**
         * @brief Checks if a texture format is a depth format.
         * @param format The texture format to check.
         * @return True if the format is a depth format, false otherwise.
         */
        bool IsDepthFormat(FrameBufferTextureFormat format) const;
    };
} // namespace BeeEngine
