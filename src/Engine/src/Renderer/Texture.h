//
// Created by alexl on 13.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "gsl/gsl"
#include "IBindable.h"
#include "BindingSet.h"
#include "Core/AssetManagement/Asset.h"

namespace BeeEngine
{
    class Texture: public IBindable, public Asset
    {
    public:
        virtual ~Texture() = default;

        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] uintptr_t GetRendererID() const { return m_RendererID; }

        bool operator==(const Texture& other) const
        {
            return m_RendererID == other.m_RendererID;
        }
        bool operator!=(const Texture& other) const
        {
            return !(*this == other);
        }
    protected:
        uintptr_t m_RendererID;
        uint32_t m_Width, m_Height;
    };

    class Texture2D: public Texture
    {
    public:
        virtual ~Texture2D() = default;

        virtual void SetData(gsl::span<std::byte> data, uint32_t numberOfChannels = 4) = 0;

        [[nodiscard]] constexpr AssetType GetType() const override
        {
            return AssetType::Texture2D;
        }

        static Ref<Texture2D> Create(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels = 4);

        BindingSet* GetBindingSet()
        {
            return m_BindingSet.get();
        }
    private:
        Ref<BindingSet> m_BindingSet = BindingSet::Create({
            {0, *this}
        });
    };
}
