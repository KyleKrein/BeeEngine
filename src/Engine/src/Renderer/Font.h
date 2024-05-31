//
// Created by alexl on 12.08.2023.
//

#pragma once
#include "Core/Path.h"
#include "Texture.h"

namespace BeeEngine
{
    namespace Internal
    {
        struct MSDFData;
    }
    class Font final : public Asset
    {
    public:
        Font(const Path& path);
        Font(const String& name, gsl::span<byte> data);
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        Font(Font&&) noexcept = default;
        Font& operator=(Font&&) noexcept = default;
        ~Font() final;

        [[nodiscard]] constexpr AssetType GetType() const override { return AssetType::Font; }

        Texture2D& GetAtlasTexture() const { return *m_AtlasTexture; }
        const Internal::MSDFData& GetMSDFData() const { return *m_Data; }

    private:
        Internal::MSDFData* m_Data = nullptr;
        Ref<Texture2D> m_AtlasTexture;

        void LoadFont(void* handle, const String& name);
    };
} // namespace BeeEngine
