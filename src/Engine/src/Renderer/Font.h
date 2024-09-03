//
// Created by alexl on 12.08.2023.
//

#pragma once
#include "Core/Path.h"
#include "JobSystem/SpinLock.h"
#include "Renderer/BindingSet.h"
#include "Texture.h"
#include <cstddef>

namespace BeeEngine
{
    namespace Internal
    {
        struct MSDFData;
    }
    /**
     * @class Font
     * @brief Represents a font asset that can be used for rendering text.
     *
     * This class handles the loading and management of font data, including
     * the generation of an atlas texture for rendering, and the associated
     * MSDF (Multi-channel Signed Distance Field) data.
     */
    class Font final : public Asset
    {
    public:
        /**
         * @brief Constructs a Font object from a file path.
         * @param path The path to the font file.
         */
        Font(const Path& path);

        /**
         * @brief Constructs a Font object from a font name and raw data.
         * @param name The name of the font.
         * @param data The raw font data as a span of bytes.
         */
        Font(const String& name, gsl::span<byte> data);

        /// Deleted copy constructor to prevent copying.
        Font(const Font&) = delete;

        /// Deleted copy assignment operator to prevent copying.
        Font& operator=(const Font&) = delete;

        /// Defaulted move constructor for efficient moving.
        Font(Font&&) noexcept = default;

        /// Defaulted move assignment operator for efficient moving.
        Font& operator=(Font&&) noexcept = default;

        /**
         * @brief Destructor that cleans up the font data and resources.
         */
        ~Font() final;

        /**
         * @brief Gets the type of the asset, which is Font.
         * @return The asset type as an enum value (AssetType::Font).
         */
        [[nodiscard]] constexpr AssetType GetType() const override { return AssetType::Font; }

        /**
         * @brief Gets the atlas texture associated with the font.
         * @return A reference to the GPUTextureResource representing the atlas texture.
         */
        GPUTextureResource& GetAtlasTexture() const { return *m_AtlasTexture; }

        /**
         * @brief Gets the MSDF data associated with the font.
         * @return A reference to the MSDFData structure.
         */
        const Internal::MSDFData& GetMSDFData() const { return *m_Data; }

        /**
         * @brief Gets the binding set for the atlas texture.
         * @return A reference to the BindingSet used for binding the atlas texture in shaders.
         */
        [[nodiscard]] BindingSet& GetAtlasBindingSet() { return *m_BindingSet; }

    private:
        Internal::MSDFData* m_Data = nullptr;
        Scope<GPUTextureResource> m_AtlasTexture;
        Ref<BindingSet> m_BindingSet;

        struct StaticData;
        static inline size_t s_Counter{0}; // A shared counter for the StaticData
        static inline Jobs::SpinLock s_Lock;
        static inline StaticData* s_Handle = nullptr;

        static void Init();
        static void Shutdown();

        void LoadFont(void* handle, const String& name);
    };
} // namespace BeeEngine
