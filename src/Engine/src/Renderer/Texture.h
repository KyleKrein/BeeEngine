//
// Created by alexl on 13.05.2023.
//

#pragma once
#include "BindingSet.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/Move.h"
#include "Core/TypeDefines.h"
#include "IBindable.h"
#include "Renderer/BindingSet.h"
#include "gsl/gsl"

namespace BeeEngine
{
    class GPUTextureResource : public IBindable
    {
    public:
        virtual ~GPUTextureResource() = default;
        virtual void SetData(gsl::span<std::byte> data, uint32_t numberOfChannels = 4) = 0;

        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] uintptr_t GetRendererID() const { return m_RendererID; }

        bool operator==(const GPUTextureResource& other) const { return m_RendererID == other.m_RendererID; }
        bool operator!=(const GPUTextureResource& other) const { return !(*this == other); }
        static Scope<GPUTextureResource>
        Create(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels = 4);
        static Scope<GPUTextureResource> Create(const Path& path);

    protected:
        uintptr_t m_RendererID;
        uint32_t m_Width, m_Height;
    };

    class Texture2D final : public Asset
    {
    public:
        Texture2D(Scope<GPUTextureResource>&& textureResource)
            : m_TextureResource(BeeMove(textureResource)), m_BindingSet(BindingSet::Create({{0, *m_TextureResource}}))
        {
        }
        [[nodiscard]] constexpr AssetType GetType() const final { return AssetType::Texture2D; }

        [[nodiscard]] const GPUTextureResource& GetGPUResource() const { return *m_TextureResource; }
        [[nodiscard]] GPUTextureResource& GetGPUResource() { return *m_TextureResource; }

        [[nodiscard]] uint32_t GetWidth() const { return m_TextureResource->GetWidth(); }
        [[nodiscard]] uint32_t GetHeight() const { return m_TextureResource->GetHeight(); }

        [[nodiscard]] BindingSet& GetBindingSet() { return *m_BindingSet; }
        static Ref<Texture2D>
        Create(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels = 4);

    private:
        Scope<GPUTextureResource> m_TextureResource;
        Ref<BindingSet> m_BindingSet;
    };
} // namespace BeeEngine
