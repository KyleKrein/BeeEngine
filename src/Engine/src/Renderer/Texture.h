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
    /**
     * @brief Represents a GPU texture resource.
     *
     * This abstract class provides the interface for a GPU texture resource,
     * which can be used in rendering pipelines and bound to shaders.
     */
    class GPUTextureResource : public IBindable
    {
    public:
        virtual ~GPUTextureResource() = default;

        /**
         * @brief Sets the texture data.
         *
         * @param data The texture data to be uploaded to the GPU.
         * @param numberOfChannels The number of color channels in the texture (default is 4).
         */
        virtual void SetData(gsl::span<std::byte> data, uint32_t numberOfChannels = 4) = 0;

        /**
         * @brief Gets the width of the texture.
         *
         * @return The width of the texture in pixels.
         */
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }

        /**
         * @brief Gets the height of the texture.
         *
         * @return The height of the texture in pixels.
         */
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }

        /**
         * @brief Gets the renderer ID of the texture for rendering with ImGui.
         *
         * @return The ImGui renderer ID of the texture.
         */
        [[nodiscard]] uintptr_t GetRendererID() const { return m_RendererID; }

        /**
         * @brief Compares this texture resource with another for equality.
         *
         * @param other The other texture resource to compare with.
         * @return True if the renderer IDs are the same, false otherwise.
         */
        bool operator==(const GPUTextureResource& other) const { return m_RendererID == other.m_RendererID; }

        /**
         * @brief Compares this texture resource with another for inequality.
         *
         * @param other The other texture resource to compare with.
         * @return True if the renderer IDs are different, false otherwise.
         */
        bool operator!=(const GPUTextureResource& other) const { return !(*this == other); }

        /**
         * @brief Creates a new GPU texture resource.
         *
         * @param width The width of the texture.
         * @param height The height of the texture.
         * @param data The initial data to upload to the texture.
         * @param numberOfChannels The number of color channels in the texture (default is 4).
         * @return A scoped pointer to the created GPU texture resource.
         */
        static Scope<GPUTextureResource>
        Create(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels = 4);

        /**
         * @brief Creates a new GPU texture resource from a file path.
         *
         * @param path The file path to load the texture from.
         * @return A scoped pointer to the created GPU texture resource.
         */
        static Scope<GPUTextureResource> Create(const Path& path);

    protected:
        uintptr_t m_RendererID; ///< Renderer ID used to bind the texture in the GPU.
        uint32_t m_Width;       ///< Width of the texture.
        uint32_t m_Height;      ///< Height of the texture.
    };

    /**
     * @brief Represents a 2D texture asset.
     *
     * This class encapsulates a GPU texture resource and provides additional functionality
     * for managing and accessing the texture as an asset within the asset management system.
     */
    class Texture2D final : public Asset
    {
    public:
        /**
         * @brief Constructs a Texture2D asset from a GPU texture resource.
         *
         * @param textureResource The GPU texture resource to manage.
         */
        Texture2D(Scope<GPUTextureResource>&& textureResource)
            : m_TextureResource(BeeMove(textureResource)), m_BindingSet(BindingSet::Create({{0, *m_TextureResource}}))
        {
        }

        /**
         * @brief Gets the type of the asset.
         *
         * @return The asset type, which is Texture2D.
         */
        [[nodiscard]] constexpr AssetType GetType() const final { return AssetType::Texture2D; }

        /**
         * @brief Gets the GPU texture resource.
         *
         * @return A constant reference to the GPU texture resource.
         */
        [[nodiscard]] const GPUTextureResource& GetGPUResource() const { return *m_TextureResource; }

        /**
         * @brief Gets the GPU texture resource.
         *
         * @return A reference to the GPU texture resource.
         */
        [[nodiscard]] GPUTextureResource& GetGPUResource() { return *m_TextureResource; }

        /**
         * @brief Gets the width of the texture.
         *
         * @return The width of the texture in pixels.
         */
        [[nodiscard]] uint32_t GetWidth() const { return m_TextureResource->GetWidth(); }

        /**
         * @brief Gets the height of the texture.
         *
         * @return The height of the texture in pixels.
         */
        [[nodiscard]] uint32_t GetHeight() const { return m_TextureResource->GetHeight(); }

        /**
         * @brief Gets the binding set associated with the texture.
         *
         * This binding set can be used to bind the texture to a shader.
         *
         * @return A reference to the binding set.
         */
        [[nodiscard]] BindingSet& GetBindingSet() { return *m_BindingSet; }

        /**
         * @brief Creates a new Texture2D asset.
         *
         * @param width The width of the texture.
         * @param height The height of the texture.
         * @param data The initial data to upload to the texture.
         * @param numberOfChannels The number of color channels in the texture (default is 4).
         * @return A reference-counted pointer to the created Texture2D asset.
         */
        static Ref<Texture2D>
        Create(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels = 4);

    private:
        Scope<GPUTextureResource> m_TextureResource; ///< The GPU texture resource managed by this asset.
        Ref<BindingSet> m_BindingSet;                ///< The binding set associated with the texture.
    };
} // namespace BeeEngine
