#include "interfaces.hpp"
#include "Core/Application.h"
#include "Core/AssetManagement/TextureImporter.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Model.h"
#include "Renderer/UniformBuffer.h"
#include "glm/ext/matrix_transform.hpp"

#define RMLUI_SDL_VERSION_MAJOR 3
#include "RmlUi_Platform_SDL.h"
#include <RmlUi/Core.h>
#include <unordered_map>

namespace BeeEngine::Internal::RmlUi
{
    struct CustomVertex
    {
        Rml::Vector2f Position;
        Color4 Color;
        Rml::Vector2f TexCoord;
    };
    struct shader_vertex_user_data_t
    {
        // Member objects are order-sensitive to match shader.
        Rml::Matrix4f m_transform = Rml::Matrix4f::Identity();
        Rml::Vector2f m_translate;
    };
    static SystemInterface_SDL* g_SystemInterface = nullptr;
    static RenderInterface* g_RenderInterface = nullptr;
    static Ref<Material> g_TextureMaterial = nullptr;
    static Ref<Material> g_ColorMaterial = nullptr;
    struct Geometry
    {
        Geometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
        {
            std::vector<uint32_t> transformedIndices;
            transformedIndices.reserve(indices.size());
            std::ranges::transform(
                indices, std::back_inserter(transformedIndices), [](int arg) { return static_cast<uint32_t>(arg); });

            std::vector<CustomVertex> transformedVertices;
            transformedVertices.reserve(vertices.size());
            std::ranges::transform(
                vertices,
                std::back_inserter(transformedVertices),
                [](const Rml::Vertex& arg)
                {
                    return CustomVertex{
                        .Position = arg.position,
                        .Color = Color4::FromRGBA(arg.colour.red, arg.colour.green, arg.colour.blue, arg.colour.alpha),
                        .TexCoord = arg.tex_coord};
                });
            Mesh = Mesh::Create({(byte*)transformedVertices.data(), sizeof(CustomVertex) * transformedVertices.size()},
                                transformedVertices.size(),
                                transformedIndices);
            ModelTexture = Model::Load(*Mesh, *g_TextureMaterial);
            ModelColor = Model::Load(*Mesh, *g_ColorMaterial);
            Buffer = UniformBuffer::Create(sizeof(shader_vertex_user_data_t));
            Set = BindingSet::Create({{.Binding = 0, .Data = *Buffer}});
        }
        Ref<Mesh> Mesh;
        Ref<Model> ModelColor;
        Ref<Model> ModelTexture;
        Ref<UniformBuffer> Buffer;
        Ref<BindingSet> Set;
    };
    struct ContextData
    {
        Scope<FrameBuffer> Framebuffer;
        Rml::Matrix4f Projection;
    };
    static std::unordered_map<UUID, Geometry> g_Geometry;
    static std::unordered_map<Rml::Context*, ContextData> g_ContextData;
    static Rml::Context* g_CurrentContext = nullptr;
    static CommandBuffer g_CurrentBuffer;
    static std::unordered_map<Rml::TextureHandle, Scope<BindingSet>> g_BindingSets;

    bool Init(void* window)
    {
        g_SystemInterface = new SystemInterface_SDL();
        g_SystemInterface->SetWindow((SDL_Window*)window);
        Rml::SetSystemInterface(g_SystemInterface);
        g_RenderInterface = new RenderInterface();
        Rml::SetRenderInterface(g_RenderInterface);
        g_TextureMaterial = Material::Create("Shaders/rmlui.vert", "Shaders/rmlui_texture.frag");
        g_ColorMaterial = Material::Create("Shaders/rmlui.vert", "Shaders/rmlui_color.frag");

        return Rml::Initialise();
    }
    Rml::Matrix4f MakeVulkanOrtho(float width, float height)
    {
        auto mat = Rml::Matrix4f::ProjectOrtho(0.0f, width, height, 0.0f, -10000, 10000);

        // https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
        Rml::Matrix4f correction_matrix;
        correction_matrix.SetColumns(Rml::Vector4f(1.0f, 0.0f, 0.0f, 0.0f),
                                     Rml::Vector4f(0.0f, -1.0f, 0.0f, 0.0f),
                                     Rml::Vector4f(0.0f, 0.0f, 0.5f, 0.0f),
                                     Rml::Vector4f(0.0f, 0.0f, 0.5f, 1.0f));

        return mat;
    }
    Rml::Context* CreateContext(const String& name, glm::i32vec2 size)
    {
        auto* result = Rml::CreateContext(name.c_str(), Rml::Vector2i(size.x, size.y));
        FrameBufferPreferences preferences;
        preferences.Width = size.x;
        preferences.Height = size.y;
        preferences.Attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth24};
        preferences.Attachments.Attachments[0].ClearColor = Color4::Transparent;
        auto& contextData = g_ContextData[result];
        contextData.Framebuffer = BeeMoveAlways(FrameBuffer::Create(BeeMove(preferences)));
        contextData.Projection = MakeVulkanOrtho(size.x, size.y);
        return result;
    }

    void ResizeFramebuffer(Rml::Context* context, glm::i32vec2 size)
    {
        BeeExpects(g_ContextData.contains(context));
        auto& contextData = g_ContextData.at(context);
        contextData.Framebuffer->Resize(size.x, size.y);
        contextData.Projection = MakeVulkanOrtho(size.x, size.y);
    }
    void SetCurrentContext(Rml::Context* context)
    {
        g_CurrentContext = context;
    }
    void BeginRendering()
    {
        BeeExpects(g_CurrentContext != nullptr && "Forgot to call SetCurrentContext(Rml::Context* context)?");
        auto& framebuffer = *g_ContextData.at(g_CurrentContext).Framebuffer;
        g_CurrentBuffer = framebuffer.Bind();
    }
    void EndRendering()
    {
        BeeExpects(g_CurrentContext != nullptr && "Forgot to call SetCurrentContext(Rml::Context* context)?");
        auto& framebuffer = *g_ContextData.at(g_CurrentContext).Framebuffer;
        framebuffer.Unbind(g_CurrentBuffer);
    }
    FrameBuffer& GetFrameBuffer(Rml::Context* context)
    {
        BeeExpects(g_ContextData.contains(context));
        return *g_ContextData.at(context).Framebuffer;
    }
    void Shutdown()
    {
        Rml::Shutdown();
        delete g_RenderInterface;
        delete g_SystemInterface;
        g_ContextData.clear();
        g_Geometry.clear();
        g_ColorMaterial.reset();
        g_TextureMaterial.reset();
        g_BindingSets.clear();
    }

    bool InputEventHandler(SDL_Window* window, SDL_Event& ev)
    {
        for (auto& [context, _] : g_ContextData)
        {
            if (!RmlSDL::InputEventHandler(context, window, ev))
                return false;
        }
        return true;
    }
    Rml::CompiledGeometryHandle RenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                                 Rml::Span<const int> indices)
    {
        UUID uuid;
        g_Geometry.insert({uuid, Geometry{vertices, indices}});
        return static_cast<uint64_t>(uuid);
    }

    void RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry,
                                         Rml::Vector2f translation,
                                         Rml::TextureHandle texture)
    {
        BeeExpects(g_Geometry.contains(geometry));
        BeeExpects(g_CurrentContext != nullptr && "Forgot to call SetCurrentContext(Rml::Context* context)?");
        auto& geometryData = g_Geometry.at(geometry);
        auto& model = texture == 0 ? *geometryData.ModelColor : *geometryData.ModelTexture;
        auto& [_, projection] = g_ContextData.at(g_CurrentContext);
        auto cmd = g_CurrentBuffer;
        shader_vertex_user_data_t vertexData{.m_transform = projection * m_CurrentTransform,
                                             .m_translate = translation};
        geometryData.Buffer->SetData(&vertexData, sizeof(shader_vertex_user_data_t));
        std::vector<BindingSet*> bindingSets = {geometryData.Set.get()};
        if (texture != 0)
        {
            bindingSets.emplace_back(g_BindingSets.at(texture).get());
        }
        cmd.SubmitInstance(model, bindingSets, {(byte*)nullptr, 0});
        cmd.Flush();
    }

    void RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
    {
        BeeExpects(g_Geometry.contains(geometry));
        g_Geometry.erase(geometry);
    }

    void RenderInterface::SetTransform(const Rml::Matrix4f* transform)
    {
        BeeCoreInfo("Called SetTransform");
        if (transform)
        {
            m_CurrentTransform = *transform;
            return;
        }
        m_CurrentTransform = Rml::Matrix4f::Identity();
    }

    Rml::TextureHandle RenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
    {
        auto texture = TextureImporter::LoadTextureFromFile(source.c_str());
        auto* ptr = texture.release();
        auto result = reinterpret_cast<uintptr_t>(ptr);
        g_BindingSets[result] = BindingSet::Create({{.Binding = 0, .Data = *ptr}});
        return result;
    }
    Rml::TextureHandle RenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source,
                                                        Rml::Vector2i source_dimensions)
    {
        std::span<byte> data = {(byte*)source.data(), source.size()};
        auto channels = static_cast<uint32_t>(source.size() / (source_dimensions.x + source_dimensions.y));
        auto texture =
            TextureImporter::LoadTextureBitmapFromMemory(data, source_dimensions.x, source_dimensions.y, channels);
        auto* ptr = texture.release();
        auto result = reinterpret_cast<uintptr_t>(ptr);
        g_BindingSets[result] = BindingSet::Create({{.Binding = 0, .Data = *ptr}});
        return result;
    }
    void RenderInterface::ReleaseTexture(Rml::TextureHandle texture)
    {
        g_BindingSets.erase(texture);
        delete (GPUTextureResource*)texture;
    }

    void RenderInterface::EnableScissorRegion(bool enable) {}
    void RenderInterface::SetScissorRegion(Rml::Rectanglei region) {}
} // namespace BeeEngine::Internal::RmlUi
