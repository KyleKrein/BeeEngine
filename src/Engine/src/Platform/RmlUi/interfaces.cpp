#include "interfaces.hpp"
#include "Core/Application.h"
#include "Core/AssetManagement/TextureImporter.h"
#include "Core/ResourceManager.h"
#include "Gui/RmlDocument.hpp"
#include "Renderer/EditorCamera.h"
#include "Renderer/Model.h"
#include "Renderer/UniformBuffer.h"
#include "glm/ext/matrix_transform.hpp"

#define RMLUI_SDL_VERSION_MAJOR 3
#include "FileSystem/File.h"
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
    static FileInterface* g_FileInterface = nullptr;
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
        Rml::Matrix4f Projection;
    };
    static std::unordered_map<UUID, Geometry> g_Geometry;
    static std::unordered_map<Rml::Context*, ContextData> g_ContextData;
    static Rml::Context* g_CurrentContext = nullptr;
    static CommandBuffer g_CurrentBuffer;
    static std::unordered_map<Rml::TextureHandle, Scope<BindingSet>> g_BindingSets;
    static std::unordered_map<Rml::TextureHandle, AssetHandle> g_Assets;

    bool Init(void* window)
    {
        g_SystemInterface = new SystemInterface_SDL();
        g_SystemInterface->SetWindow((SDL_Window*)window);
        Rml::SetSystemInterface(g_SystemInterface);
        g_RenderInterface = new RenderInterface();
        Rml::SetRenderInterface(g_RenderInterface);
        g_FileInterface = new FileInterface();
        Rml::SetFileInterface(g_FileInterface);
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
        auto& contextData = g_ContextData[result];
        contextData.Projection = MakeVulkanOrtho(size.x, size.y);
        return result;
    }

    void ResizeViewport(Rml::Context* context, glm::i32vec2 size)
    {
        BeeExpects(g_ContextData.contains(context));
        auto& contextData = g_ContextData.at(context);
        contextData.Projection = MakeVulkanOrtho(size.x, size.y);
        context->SetDimensions({size.x, size.y});
    }
    void SetCurrentContext(Rml::Context* context)
    {
        g_CurrentContext = context;
    }
    Rml::Context* GetCurrentContext()
    {
        return g_CurrentContext;
    }
    void SetLocaleDomain(Locale::Domain* domain)
    {
        g_SystemInterface->SetLocalizationDomain(*domain);
    }
    void BeginRendering(CommandBuffer& cmd)
    {
        BeeExpects(g_CurrentContext != nullptr && "Forgot to call SetCurrentContext(Rml::Context* context)?");
        g_CurrentBuffer = cmd;
    }
    void EndRendering()
    {
        BeeExpects(g_CurrentContext != nullptr && "Forgot to call SetCurrentContext(Rml::Context* context)?");
        g_CurrentBuffer = {};
    }
    void Shutdown()
    {
        Rml::Shutdown();
        delete g_FileInterface;
        delete g_RenderInterface;
        delete g_SystemInterface;
        g_ContextData.clear();
        g_Geometry.clear();
        g_ColorMaterial.reset();
        g_TextureMaterial.reset();
        g_BindingSets.clear();
        g_Assets.clear();
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
        auto& [projection] = g_ContextData.at(g_CurrentContext);
        auto cmd = g_CurrentBuffer;
        shader_vertex_user_data_t vertexData{.m_transform = projection * m_CurrentTransform,
                                             .m_translate = translation};
        geometryData.Buffer->SetData(&vertexData, sizeof(shader_vertex_user_data_t));
        std::vector<BindingSet*> bindingSets = {geometryData.Set.get()};
        int32_t flipUv = 0;
        if (texture != 0)
        {
            if (g_Assets.contains(texture))
            {
                flipUv = 1;
                bindingSets.emplace_back(
                    &AssetManager::GetAsset<Texture2D>(g_Assets.at(texture), Locale::Localization::Default)
                         .GetBindingSet());
            }
            else
            {
                bindingSets.emplace_back(g_BindingSets.at(texture).get());
            }
        }
        cmd.SubmitInstance(model, bindingSets, {(byte*)&flipUv, sizeof(int32_t)});
        cmd.Flush();
    }

    void RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
    {
        BeeExpects(g_Geometry.contains(geometry));
        g_Geometry.erase(geometry);
    }

    void RenderInterface::SetTransform(const Rml::Matrix4f* transform)
    {
        BeeCoreTrace("Called RenderInterface::SetTransform");
        if (transform)
        {
            m_CurrentTransform = *transform;
            return;
        }
        m_CurrentTransform = Rml::Matrix4f::Identity();
    }

    Rml::TextureHandle RenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
    {
        auto* asset = &AssetManager::GetAsset<Texture2D>(Path{source.c_str()}, Locale::Localization::Default);
        if (!asset)
        {
            return 0;
        }
        Rml::TextureHandle result = UUID{};
        BeeExpects(!g_Assets.contains(result) &&
                   "This should never happen. If it happens, your are unlucky. Also it can mean, that using one range "
                   "of uint64_t for both pointers and UUIDs is not enough. Please make an Issue on github and attach a "
                   "screenshot of this message");
        g_Assets[result] = asset->Handle;
        texture_dimensions = {static_cast<int>(asset->GetWidth()), static_cast<int>(asset->GetHeight())};
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
        BeeEnsures(!g_Assets.contains(result) &&
                   "This should never happen. If it happens, your are unlucky. Also it can mean, that using one range "
                   "of uint64_t for both pointers and UUIDs is not enough. Please make an Issue on github and attach a "
                   "screenshot of this message");
        return result;
    }
    void RenderInterface::ReleaseTexture(Rml::TextureHandle texture)
    {
        BeeExpects((g_Assets.contains(texture) || g_BindingSets.contains(texture)) &&
                   !(g_Assets.contains(texture) && g_BindingSets.contains(texture)) &&
                   "This should never happen. If it happens, your are unlucky. Also it can mean, that using one range "
                   "of uint64_t for both pointers and UUIDs is not enough. Please make an Issue on github and attach a "
                   "screenshot of this message");
        if (g_Assets.contains(texture))
        {
            g_Assets.erase(texture);
            return;
        }
        g_BindingSets.erase(texture);
        delete (GPUTextureResource*)texture;
    }

    void RenderInterface::EnableScissorRegion(bool enable) {}
    void RenderInterface::SetScissorRegion(Rml::Rectanglei region) {}

    Rml::FileHandle FileInterface::Open(const Rml::String& pathStr)
    {
        Path path{pathStr};
        BeeCoreInfo("Open {}", path);
        String content;
        try
        {
            if (ResourceManager::IsRcssExtension(path.GetExtension()))
            {
                content = AssetManager::GetAsset<Rcss>(path).GetFileContent();
            }
            else if (ResourceManager::IsRmlExtension(path.GetExtension()))
            {
                content = AssetManager::GetAsset<RmlDocument>(path).GetFileContent();
            }
            else
            {
                content = File::ReadFile(path);
            }
        }
        catch (const std::bad_cast& exception)
        {
            BeeCoreWarn("Unable to load {}. Check, if the name of the asset is correct", pathStr);
            return 0;
        }
        catch (...)
        {
            BeeCoreError("Unknown error when trying to load {}", pathStr);
            return 0;
        }
        if (content.empty())
        {
            return 0;
        }
        Rml::FileHandle handle = UUID{};
        m_FilesInFlight[handle] = VirtualFile{.content = content};
        return handle;
    }

    void FileInterface::Close(Rml::FileHandle file)
    {
        m_FilesInFlight.erase(file);
    }

    size_t FileInterface::Read(void* buffer, size_t size, Rml::FileHandle file)
    {
        auto it = m_FilesInFlight.find(file);
        if (it == m_FilesInFlight.end())
        {
            return 0;
        }

        VirtualFile& virtualFile = it->second;
        size_t remaining = virtualFile.content.size() - virtualFile.position;
        size_t toRead = std::min(size, remaining);
        memcpy(buffer, virtualFile.content.data() + virtualFile.position, toRead);
        virtualFile.position += toRead;
        return toRead;
    }

    bool FileInterface::Seek(Rml::FileHandle file, long offset, int origin)
    {
        auto it = m_FilesInFlight.find(file);
        if (it == m_FilesInFlight.end())
        {
            return false;
        }

        VirtualFile& virtualFile = it->second;
        size_t newPos = 0;

        switch (origin)
        {
            case SEEK_SET:
                newPos = offset;
                break;
            case SEEK_CUR:
                newPos = virtualFile.position + offset;
                break;
            case SEEK_END:
                newPos = virtualFile.content.size() + offset;
                break;
            default:
                return false;
        }

        if (newPos > virtualFile.content.size())
        {
            return false;
        }

        virtualFile.position = newPos;
        return true;
    }

    size_t FileInterface::Tell(Rml::FileHandle file)
    {
        auto it = m_FilesInFlight.find(file);
        if (it == m_FilesInFlight.end())
        {
            return -1;
        }

        return it->second.position;
    }

    bool FileInterface::LoadFile(const String& pathStr, String& out_data)
    {
        Path path = {pathStr};
        BeeCoreInfo("Loading file from {}", path);
        if (ResourceManager::IsRcssExtension(path.GetExtension()))
        {
            out_data = AssetManager::GetAsset<Rcss>(path).GetFileContent();
            return !out_data.empty();
        }
        out_data = File::ReadFile(Path{path});
        return !out_data.empty();
    }

} // namespace BeeEngine::Internal::RmlUi
