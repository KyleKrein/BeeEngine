//
// Created by alexl on 15.07.2023.
//

#include "WebGPUTexture2D.h"
#include "WebGPUGraphicsDevice.h"
#include <stb_image.h>

namespace BeeEngine::Internal
{

    WebGPUTexture2D::WebGPUTexture2D(std::string_view path)
    {
        int width, height, channels;
        unsigned char *data = stbi_load(path.data(), &width, &height, &channels, 0);
        if (!data)
        {
            BeeCoreError("Failed to load image: {0}", path);
            throw std::exception();
        }
        m_Width = width;
        m_Height = height;
        WGPUDevice device;
        WGPUTextureDescriptor textureDesc;
        CreateTexture(width, height, device, textureDesc);

        // Upload data to the GPU texture (to be implemented!)
        WriteMipMaps(device, m_Texture, textureDesc.size, textureDesc.mipLevelCount, data);

        CreateTextureView(textureDesc);

        stbi_image_free(data);
    }

    void WebGPUTexture2D::CreateTextureView(const WGPUTextureDescriptor &textureDesc)
    {
        WGPUTextureViewDescriptor textureViewDesc;
        textureViewDesc.nextInChain = nullptr;
        textureViewDesc.label = "Texture2DView";
        textureViewDesc.aspect = WGPUTextureAspect_All;
        textureViewDesc.baseArrayLayer = 0;
        textureViewDesc.arrayLayerCount = 1;
        textureViewDesc.baseMipLevel = 0;
        textureViewDesc.mipLevelCount = textureDesc.mipLevelCount;
        textureViewDesc.dimension = WGPUTextureViewDimension_2D;
        textureViewDesc.format = textureDesc.format;
        m_TextureView = wgpuTextureCreateView(m_Texture, &textureViewDesc);
    }

    void WebGPUTexture2D::CreateTexture(int width, int height, WGPUDevice &device, WGPUTextureDescriptor &textureDesc)
    {
        device= WebGPUGraphicsDevice::GetInstance().GetDevice();
        textureDesc= {};
        textureDesc.nextInChain = nullptr;
        textureDesc.label = "Texture2D";
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.format = WGPUTextureFormat_RGBA8Unorm; // by convention for bmp, png and jpg file. Be careful with other formats.
        textureDesc.sampleCount = 1;
        textureDesc.size = { (unsigned int)width, (unsigned int)height, 1 };
        textureDesc.mipLevelCount = std::bit_width(std::max(textureDesc.size.width, textureDesc.size.height));
        textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
        textureDesc.viewFormatCount = 0;
        textureDesc.viewFormats = nullptr;
        m_Texture = wgpuDeviceCreateTexture(device, &textureDesc);
    }

    WebGPUTexture2D::WebGPUTexture2D(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        WGPUDevice device;
        WGPUTextureDescriptor textureDesc;
        CreateTexture(m_Width, m_Height, device, textureDesc);
        CreateTextureView(textureDesc);
    }

    WebGPUTexture2D::WebGPUTexture2D(gsl::span<std::byte> dataFromMemory)
    {
        int width, height, channels;
        stbi_uc* data = nullptr;
        {
            data = stbi_load_from_memory(reinterpret_cast<stbi_uc*>(dataFromMemory.data()), gsl::narrow_cast<int>(dataFromMemory.size()), &width, &height, &channels, 0);
        }
        if (!data)
        {
            BeeCoreError("Failed to load image from memory!");
            throw std::exception();//return;
        }
        m_Width = width;
        m_Height = height;
        WGPUDevice device;
        WGPUTextureDescriptor textureDesc;
        CreateTexture(width, height, device, textureDesc);
        WriteMipMaps(device, m_Texture, textureDesc.size, textureDesc.mipLevelCount, data);
        CreateTextureView(textureDesc);
        stbi_image_free(data);
    }

    WebGPUTexture2D::~WebGPUTexture2D()
    {
        wgpuTextureViewRelease(m_TextureView);
        wgpuTextureDestroy(m_Texture);
        wgpuTextureRelease(m_Texture);
    }

    void WebGPUTexture2D::Bind(uint32_t slot)
    {

    }

    void WebGPUTexture2D::SetData(gsl::span<std::byte> data)
    {
        WriteMipMaps(WebGPUGraphicsDevice::GetInstance().GetDevice(), m_Texture, { m_Width, m_Height, 1 }, 1, (const unsigned char *)data.data());
    }

    void WebGPUTexture2D::WriteMipMaps(WGPUDevice device,
                                       WGPUTexture texture,
                                       WGPUExtent3D textureSize,
                                       uint32_t mipLevelCount,
                                       const unsigned char *data)
    {
        WGPUQueue queue = wgpuDeviceGetQueue(device);

        WGPUImageCopyTexture destination = {};
        destination.texture = texture;
        destination.origin = { 0, 0, 0 };
        destination.aspect = WGPUTextureAspect_All;

        WGPUTextureDataLayout source = {};
        source.offset = 0;

        // Create image data
        WGPUExtent3D mipLevelSize = textureSize;
        std::vector<unsigned char> previousLevelPixels;
        WGPUExtent3D previousMipLevelSize;
        for (uint32_t level = 0; level < mipLevelCount; ++level) {
            // Pixel data for the current level
            std::vector<unsigned char> pixels(4 * mipLevelSize.width * mipLevelSize.height);
            if (level == 0) {
                // We cannot really avoid this copy since we need this
                // in previousLevelPixels at the next iteration
                memcpy(pixels.data(), data, pixels.size());
            }
            else {
                // Create mip level data
                for (uint32_t i = 0; i < mipLevelSize.width; ++i) {
                    for (uint32_t j = 0; j < mipLevelSize.height; ++j) {
                        unsigned char* p = &pixels[4 * (j * mipLevelSize.width + i)];
                        // Get the corresponding 4 pixels from the previous level
                        unsigned char* p00 = &previousLevelPixels[4 * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 0))];
                        unsigned char* p01 = &previousLevelPixels[4 * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 1))];
                        unsigned char* p10 = &previousLevelPixels[4 * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 0))];
                        unsigned char* p11 = &previousLevelPixels[4 * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 1))];
                        // Average
                        p[0] = (p00[0] + p01[0] + p10[0] + p11[0]) / 4;
                        p[1] = (p00[1] + p01[1] + p10[1] + p11[1]) / 4;
                        p[2] = (p00[2] + p01[2] + p10[2] + p11[2]) / 4;
                        p[3] = (p00[3] + p01[3] + p10[3] + p11[3]) / 4;
                    }
                }
            }
            // Upload data to the GPU texture
            destination.mipLevel = level;
            source.bytesPerRow = 4 * mipLevelSize.width;
            source.rowsPerImage = mipLevelSize.height;
            wgpuQueueWriteTexture(queue, &destination, pixels.data(), pixels.size(), &source, &mipLevelSize);

            previousLevelPixels = std::move(pixels);
            previousMipLevelSize = mipLevelSize;
            mipLevelSize.width /= 2;
            mipLevelSize.height /= 2;
        }
        wgpuQueueRelease(queue);
    }

    WGPUBindGroupLayoutEntry WebGPUTexture2D::GetBindGroupLayoutEntry() const
    {
        WGPUBindGroupLayoutEntry entry = {};
        WebGPUGraphicsDevice::GetInstance().SetDefault(entry);
        entry.visibility = WGPUShaderStage_Fragment;
        entry.texture.sampleType = WGPUTextureSampleType_Float;
        entry.texture.viewDimension = WGPUTextureViewDimension_2D;
        return entry;
    }

    WGPUBindGroupEntry WebGPUTexture2D::GetBindGroupEntry() const
    {
        WGPUBindGroupEntry entry = {};
        WebGPUGraphicsDevice::GetInstance().SetDefault(entry);
        entry.textureView = m_TextureView;
        return entry;
    }
}