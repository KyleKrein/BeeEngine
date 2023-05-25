//
// Created by alexl on 13.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetRendererID() const { return m_RendererID; }

        virtual void Bind(uint32_t slot = 0) const = 0;

        bool operator==(const Texture& other) const
        {
            return m_RendererID == other.m_RendererID;
        }
        bool operator!=(const Texture& other) const
        {
            return !(*this == other);
        }
    protected:
        uint32_t m_RendererID;
        uint32_t m_Width, m_Height;
    };

    class Texture2D: public Texture
    {
    public:
        virtual ~Texture2D() = default;

        virtual void SetData(void* data, uint32_t size) = 0;

        static Ref<Texture2D> Create(std::string_view path);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
    };
}
