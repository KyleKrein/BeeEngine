//
// Created by alexl on 12.08.2023.
//

#pragma once
#include <filesystem>
#include "Texture.h"

namespace BeeEngine
{
    namespace Internal
    {
        struct MSDFData;
    }
    class Font
    {
    public:
        Font(const std::filesystem::path& path);
        Font(const std::string& name,gsl::span<byte> data);
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        Font(Font&&) noexcept = default;
        Font& operator=(Font&&) noexcept = default;
        ~Font();

        Texture2D& GetAtlasTexture() const { return *m_AtlasTexture; }
        const Internal::MSDFData& GetMSDFData() const { return *m_Data; }

    private:
        Internal::MSDFData* m_Data = nullptr;
        Ref<Texture2D> m_AtlasTexture;

        void LoadFont(void *handle, const std::string& name);
    };
}
