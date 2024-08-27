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

        GPUTextureResource& GetAtlasTexture() const { return *m_AtlasTexture; }
        const Internal::MSDFData& GetMSDFData() const { return *m_Data; }

        [[nodiscard]] BindingSet& GetAtlasBindingSet() { return *m_BindingSet; }

    private:
        Internal::MSDFData* m_Data = nullptr;
        Scope<GPUTextureResource> m_AtlasTexture;
        Ref<BindingSet> m_BindingSet;

        struct StaticData;
        static inline size_t s_Counter{0};
        static inline Jobs::SpinLock s_Lock;
        static inline StaticData* s_Handle = nullptr;

        static void Init();
        static void Shutdown();

        void LoadFont(void* handle, const String& name);
    };
} // namespace BeeEngine
