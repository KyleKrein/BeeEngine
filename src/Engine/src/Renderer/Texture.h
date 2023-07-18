//
// Created by alexl on 13.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "gsl/gsl"
#include "IBindable.h"

namespace BeeEngine
{
    class Texture: public IBindable
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

        virtual void SetData(gsl::span<std::byte> data) = 0;

        static Ref<Texture2D> Create(std::string_view path);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
        static Ref<Texture2D> CreateFromMemory(gsl::span<std::byte> data);
    };
}
