//
// Created by alexl on 15.07.2023.
//

#include "WebGPUTexture2D.h"
#include "WebGPUGraphicsDevice.h"
#include <version>
#include <cmath>
#include <bit>

namespace BeeEngine::Internal
{
    WebGPUTexture2D::WebGPUTexture2D(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels)
    {
        m_Width = width;
        m_Height = height;
        WGPUDevice device;
        WGPUTextureDescriptor textureDesc;
        CreateTextureAndSampler(m_Width, m_Height, device, textureDesc);

        m_MipLevels = textureDesc.mipLevelCount;

        SetData(data, numberOfChannels);
        CreateTextureView(textureDesc);
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
        m_RendererID = reinterpret_cast<uintptr_t>(m_TextureView);
    }

#if !defined(__cpp_lib_int_pow2)
    template<typename T>
    T bit_width(T value)
    {
        return value == 0? 0:log2(value) + 1;
    }
#endif

    void WebGPUTexture2D::CreateTextureAndSampler(int width, int height, WGPUDevice &device, WGPUTextureDescriptor &textureDesc)
    {
        device= WebGPUGraphicsDevice::GetInstance().GetDevice();
        textureDesc= {};
        textureDesc.nextInChain = nullptr;
        textureDesc.label = "Texture2D";
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.format = WGPUTextureFormat_RGBA8Unorm; // by convention for bmp, png and jpg file. Be careful with other formats.
        textureDesc.sampleCount = 1;
        textureDesc.size = { (unsigned int)width, (unsigned int)height, 1 };
#if !defined(__cpp_lib_int_pow2)
        textureDesc.mipLevelCount = bit_width(std::max(textureDesc.size.width, textureDesc.size.height));
#else
        textureDesc.mipLevelCount = std::bit_width(std::max(textureDesc.size.width, textureDesc.size.height));
#endif
        textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
        textureDesc.viewFormatCount = 0;
        textureDesc.viewFormats = nullptr;
        m_Texture = wgpuDeviceCreateTexture(device, &textureDesc);


        // Create a sampler
        WGPUSamplerDescriptor samplerDesc;
        samplerDesc.nextInChain = nullptr;
        samplerDesc.label = "Texture2DSampler";
        samplerDesc.addressModeU = WGPUAddressMode_Repeat;
        samplerDesc.addressModeV = WGPUAddressMode_Repeat;
        samplerDesc.addressModeW = WGPUAddressMode_Repeat;
        samplerDesc.magFilter = WGPUFilterMode_Linear;
        samplerDesc.minFilter = WGPUFilterMode_Linear;
        samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Linear;
        samplerDesc.lodMinClamp = 0.0f;
        samplerDesc.lodMaxClamp = 1.0f;
        samplerDesc.compare = WGPUCompareFunction_Undefined;
        samplerDesc.maxAnisotropy = 1;
        m_Sampler = wgpuDeviceCreateSampler(device, &samplerDesc);
    }

    WebGPUTexture2D::~WebGPUTexture2D()
    {
        wgpuSamplerRelease(m_Sampler);
        if(!m_Texture)
            return;
        wgpuTextureViewRelease(m_TextureView);
        wgpuTextureDestroy(m_Texture);
        wgpuTextureRelease(m_Texture);
    }

    void WebGPUTexture2D::Bind(uint32_t slot)
    {

    }

    void WebGPUTexture2D::SetData(gsl::span<std::byte> data, uint32_t numberOfChannels)
    {
        if(numberOfChannels == 3)
        {
            std::vector<std::byte> dataWithAlpha;
            dataWithAlpha.resize(data.size() * 4 / 3);
            for(int i = 0; i < data.size() / 3; i++)
            {
                dataWithAlpha[i * 4] = data[i * 3];
                dataWithAlpha[i * 4 + 1] = data[i * 3 + 1];
                dataWithAlpha[i * 4 + 2] = data[i * 3 + 2];
                dataWithAlpha[i * 4 + 3] = std::byte(255);
            }
            WriteMipMaps(WebGPUGraphicsDevice::GetInstance().GetDevice(), m_Texture, { m_Width, m_Height, 1 }, m_MipLevels, (const unsigned char *)dataWithAlpha.data());
            return;
        }
        WriteMipMaps(WebGPUGraphicsDevice::GetInstance().GetDevice(), m_Texture, { m_Width, m_Height, 1 }, m_MipLevels, (const unsigned char *)data.data());
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

    std::vector<WGPUBindGroupLayoutEntry> WebGPUTexture2D::GetBindGroupLayoutEntry() const
    {
        WGPUBindGroupLayoutEntry textureEntry = {};
        WebGPUGraphicsDevice::GetInstance().SetDefault(textureEntry);
        textureEntry.visibility = WGPUShaderStage_Fragment;
        textureEntry.texture.sampleType = WGPUTextureSampleType_Float;
        textureEntry.texture.viewDimension = WGPUTextureViewDimension_2D;

        WGPUBindGroupLayoutEntry samplerEntry = {};
        WebGPUGraphicsDevice::GetInstance().SetDefault(samplerEntry);
        samplerEntry.visibility = WGPUShaderStage_Fragment;
        samplerEntry.sampler.type = WGPUSamplerBindingType_Filtering;
        return { textureEntry, samplerEntry};
    }

    std::vector<WGPUBindGroupEntry> WebGPUTexture2D::GetBindGroupEntry() const
    {
        WGPUBindGroupEntry textureEntry = {};
        WebGPUGraphicsDevice::GetInstance().SetDefault(textureEntry);
        textureEntry.textureView = m_TextureView;

        WGPUBindGroupEntry samplerEntry = {};
        WebGPUGraphicsDevice::GetInstance().SetDefault(samplerEntry);
        samplerEntry.sampler = m_Sampler;
        return { textureEntry, samplerEntry };
    }

    WebGPUTexture2D::WebGPUTexture2D(WGPUTextureView textureView, uint32_t width, uint32_t height)
    {
        m_TextureView = textureView;
        m_Width = width;
        m_Height = height;
        auto device = WebGPUGraphicsDevice::GetInstance().GetDevice();
        WGPUSamplerDescriptor samplerDesc;
        samplerDesc.nextInChain = nullptr;
        samplerDesc.label = "Texture2DSampler";
        samplerDesc.addressModeU = WGPUAddressMode_ClampToEdge;
        samplerDesc.addressModeV = WGPUAddressMode_ClampToEdge;
        samplerDesc.addressModeW = WGPUAddressMode_ClampToEdge;
        samplerDesc.magFilter = WGPUFilterMode_Linear;
        samplerDesc.minFilter = WGPUFilterMode_Linear;
        samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Linear;
        samplerDesc.lodMinClamp = 0.0f;
        samplerDesc.lodMaxClamp = 1.0f;
        samplerDesc.compare = WGPUCompareFunction_Undefined;
        samplerDesc.maxAnisotropy = 1;
        m_Sampler = wgpuDeviceCreateSampler(device, &samplerDesc);
    }
}
