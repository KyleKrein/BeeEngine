//
// Created by alexl on 15.07.2023.
//

#pragma once

#include "Renderer/Texture.h"

namespace BeeEngine::Internal
{
    class WebGPUTexture2D final: public Texture2D
    {
    public:
        WebGPUTexture2D(std::string_view path);
        WebGPUTexture2D(uint32_t width, uint32_t height);
        WebGPUTexture2D(gsl::span<std::byte> data);
        ~WebGPUTexture2D() override;

        void Bind(uint32_t slot = 0) override;
        WGPUBindGroupLayoutEntry GetBindGroupLayoutEntry() const override;
        WGPUBindGroupEntry GetBindGroupEntry() const override;
        void SetData(gsl::span<std::byte> data) override;
    private:
        void WriteMipMaps(WGPUDevice pDevice, WGPUTexture pTexture, WGPUExtent3D extent3D, uint32_t count,
                          const unsigned char *data);

        WGPUTexture m_Texture;
        WGPUTextureView m_TextureView;

        void CreateTexture(int width, int height, WGPUDevice &device, WGPUTextureDescriptor &textureDesc);

        void CreateTextureView(const WGPUTextureDescriptor &textureDesc);
    };
}