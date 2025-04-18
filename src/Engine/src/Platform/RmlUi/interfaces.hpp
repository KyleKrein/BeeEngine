#pragma once

#include "Core/String.h"
#include "Renderer/FrameBuffer.h"
#include "RmlUi/Core/FileInterface.h"
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/SystemInterface.h>
#include <glm/glm.hpp>

namespace BeeEngine::Internal::RmlUi
{
    bool Init(void* window);
    Rml::Context* CreateContext(const String& name, glm::i32vec2 sizeInPixels);
    void SetCurrentContext(Rml::Context* context);
    Rml::Context* GetCurrentContext();
    void BeginRendering(CommandBuffer& cmd);
    void EndRendering();
    void ResizeViewport(Rml::Context* context, glm::i32vec2 size);
    void Shutdown();

    class RenderInterface : public Rml::RenderInterface
    {
    public:
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
        void SetTransform(const Rml::Matrix4f* transform) override;

    private:
        Rml::Matrix4f m_CurrentTransform = Rml::Matrix4f::Identity();
    };
    class FileInterface final : public Rml::FileInterface
    {
    public:
        // Opens a file.
        Rml::FileHandle Open(const Rml::String& path) final;

        // Closes a previously opened file.
        void Close(Rml::FileHandle file) final;

        // Reads data from a previously opened file.
        size_t Read(void* buffer, size_t size, Rml::FileHandle file) final;

        // Seeks to a point in a previously opened file.
        bool Seek(Rml::FileHandle file, long offset, int origin) final;

        // Returns the current position of the file pointer.
        size_t Tell(Rml::FileHandle file) final;
        virtual bool LoadFile(const String& path, String& out_data) final;

    private:
        struct VirtualFile
        {
            String content;
            size_t position = 0;
        };
        std::unordered_map<Rml::FileHandle, VirtualFile> m_FilesInFlight;
    };
} // namespace BeeEngine::Internal::RmlUi
