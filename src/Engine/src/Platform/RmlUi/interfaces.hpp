#pragma once

#include "Core/String.h"
#include "Renderer/FrameBuffer.h"
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/SystemInterface.h>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

namespace BeeEngine::Internal::RmlUi
{
    bool Init(void* window);
    Rml::Context* CreateContext(const String& name, glm::i32vec2 size);
    void SetCurrentContext(Rml::Context* context);
    FrameBuffer& GetFrameBuffer(Rml::Context* context);
    void BeginRendering();
    void EndRendering();
    void ResizeFramebuffer(Rml::Context* context, glm::i32vec2 size);
    void Shutdown();

    // Applies input on all contexts based on the given SDL event.
    // @return True if the event is still propagating, false if it was handled by the context.
    bool InputEventHandler(SDL_Window* window, SDL_Event& ev);

    class RenderInterface : public Rml::RenderInterface
    {
        Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                    Rml::Span<const int> indices) override;
        void RenderGeometry(Rml::CompiledGeometryHandle geometry,
                            Rml::Vector2f translation,
                            Rml::TextureHandle texture) override;
        void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

        Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
        Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
        void ReleaseTexture(Rml::TextureHandle texture) override;

        void EnableScissorRegion(bool enable) override;
        void SetScissorRegion(Rml::Rectanglei region) override;
    };
} // namespace BeeEngine::Internal::RmlUi
