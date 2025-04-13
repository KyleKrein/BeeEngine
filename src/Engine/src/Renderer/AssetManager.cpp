//
// Created by Александр Лебедев on 27.06.2023.
//

#include "AssetManager.h"
#include "../../Assets/EmbeddedResources.h"
#include "Vertex.h"
#include <glm/glm.hpp>
#include <span>

BeeEngine::Material& BeeEngine::InternalAssetManager::LoadMaterial(const String& name,
                                                                   const std::filesystem::path& vertexShader,
                                                                   const std::filesystem::path& fragmentShader)
{
    if (HasMaterial(name))
        return GetMaterial(name);
    else
        return *m_Materials.emplace(name, Material::Create(vertexShader, fragmentShader)).first->second;
}

BeeEngine::Mesh& BeeEngine::InternalAssetManager::LoadMesh(const String& name, const std::filesystem::path& path)
{
    if (HasMesh(name))
        return GetMesh(name);
    else
    {
        std::vector<BeeEngine::Vertex> vertices;
        std::vector<uint32_t> indices;
       // auto result = Internal::ModelLoader::LoadglTF(path.string(), vertices, indices);
        //BeeEnsures(result);
      BeeCoreError("Loading of meshes using internal Asset manager is not supported");
      BeeEnsures(false);
        return *m_Meshes.emplace(name, Mesh::Create(vertices, indices)).first->second;
    }
}

BeeEngine::Mesh& BeeEngine::InternalAssetManager::LoadMesh(const String& name,
                                                           std::vector<BeeEngine::Vertex>& vertices,
                                                           std::vector<uint32_t>& indices)
{
    if (HasMesh(name))
        return GetMesh(name);
    else
        return *m_Meshes.emplace(name, Mesh::Create(vertices, indices)).first->second;
}
template <typename VertexType>
BeeEngine::Mesh& BeeEngine::InternalAssetManager::LoadMesh(const String& name,
                                                           std::vector<VertexType>& vertices,
                                                           std::vector<uint32_t>& indices)
{
    if (HasMesh(name))
        return GetMesh(name);
    size_t vertexCount = vertices.size();
    size_t size = vertexCount * sizeof(VertexType);
    void* data = (void*)vertices.data();
    return *m_Meshes.emplace(name, Mesh::Create(data, size, vertexCount, indices)).first->second;
}

BeeEngine::Texture2D& BeeEngine::InternalAssetManager::LoadTexture(const String& name,
                                                                   const std::filesystem::path& path)
{
    if (HasTexture(name))
    {
        return GetTexture(name);
    }
    BeeCoreAssert(false, "Not implemented!");
    // return *m_Textures.emplace(name, Texture2D::Create(path.string())).first->second;
}

BeeEngine::Material& BeeEngine::InternalAssetManager::GetMaterial(const String& name)
{
    BeeExpects(HasMaterial(name));
    return *m_Materials.at(name);
}

BeeEngine::Mesh& BeeEngine::InternalAssetManager::GetMesh(const String& name)
{
    BeeExpects(HasMesh(name));
    return *m_Meshes.at(name);
}

BeeEngine::Texture2D& BeeEngine::InternalAssetManager::GetTexture(const String& name)
{
    BeeExpects(HasTexture(name));
    return *m_Textures.at(name);
}

bool BeeEngine::InternalAssetManager::HasMaterial(const String& name) const
{
    if (m_Materials.find(name) != m_Materials.end())
        return true;
    else
        return false;
}

bool BeeEngine::InternalAssetManager::HasMesh(const String& name) const
{
    if (m_Meshes.find(name) != m_Meshes.end())
        return true;
    else
        return false;
}

bool BeeEngine::InternalAssetManager::HasTexture(const String& name) const
{
    if (m_Textures.find(name) != m_Textures.end())
        return true;
    else
        return false;
}

BeeEngine::Model&
BeeEngine::InternalAssetManager::LoadModel(const String& name, BeeEngine::Material& material, BeeEngine::Mesh& mesh)
{
    if (HasModel(name))
        return GetModel(name);
    else
        return *m_Models.emplace(name, Model::Load(mesh, material)).first->second;
}

BeeEngine::Model& BeeEngine::InternalAssetManager::GetModel(const String& name)
{
    BeeExpects(HasModel(name));
    return *m_Models.at(name);
}

bool BeeEngine::InternalAssetManager::HasModel(const String& name) const
{
    if (m_Models.find(name) != m_Models.end())
        return true;
    else
        return false;
}

BeeEngine::Texture2D&
BeeEngine::InternalAssetManager::LoadTexture(const String& name, uint32_t width, uint32_t height, std::span<byte> data)
{
    m_Textures.emplace(name, Texture2D::Create(width, height, data));
    return *m_Textures.at(name);
}

BeeEngine::Font& BeeEngine::InternalAssetManager::LoadFont(const String& name, const std::filesystem::path& path)
{
    if (HasFont(name))
        return GetFont(name);
    else
        return *m_Fonts.emplace(name, CreateRef<Font>(path)).first->second;
}

BeeEngine::Font& BeeEngine::InternalAssetManager::LoadFont(const String& name, std::span<byte> data)
{
    if (HasFont(name))
        return GetFont(name);
    else
        return *m_Fonts.emplace(name, CreateRef<Font>(name, data)).first->second;
}

BeeEngine::Font& BeeEngine::InternalAssetManager::GetFont(const String& name)
{
    BeeExpects(HasFont(name));
    return *m_Fonts.at(name);
}

bool BeeEngine::InternalAssetManager::HasFont(const String& name) const
{
    return m_Fonts.contains(name);
}

void BeeEngine::InternalAssetManager::LoadStandardAssets()
{
    using namespace BeeEngine;
    uint32_t data = 0xffffffff;
    auto& blank = LoadTexture("Blank", 1, 1, {(byte*)&data, 4});

    // checkerboard image
    uint32_t magenta = 0xFF00FFFF;
    uint32_t black = 0x000000FF;
    std::array<uint32_t, 16 * 16> pixels; // for 16x16 checkerboard texture
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }
    auto& checkerboard = LoadTexture("Checkerboard", 16, 16, {(byte*)pixels.data(), 16 * 16 * 4});

    auto& spriteMaterial = LoadMaterial(
        "Renderer2D_SpriteMaterial", "Shaders/Renderer2D_SpriteShader.vert", "Shaders/Renderer2D_SpriteShader.frag");

    std::vector<BeeEngine::Vertex> vertexBuffer = {{
                                                       {-0.5f, -0.5f, 0.0f},
                                                       {1.0f, 1.0f, 1.0f},
                                                       {0.0f, 0.0f},
                                                   },
                                                   {
                                                       {0.5f, -0.5f, 0.0f},
                                                       {1.0f, 1.0f, 1.0f},
                                                       {1.0f, 0.0f},
                                                   },
                                                   {
                                                       {0.5f, 0.5f, 0.0f},
                                                       {1.0f, 1.0f, 1.0f},
                                                       {1.0f, 1.0f},
                                                   },
                                                   {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};
    std::vector<uint32_t> indexBuffer = {2, 1, 0, 0, 3, 2};
    auto& mesh = LoadMesh("Renderer2D_RectangleMesh", vertexBuffer, indexBuffer);
    auto& rectModel = LoadModel("Renderer2D_Rectangle", spriteMaterial, mesh);

    auto& circleMaterial = LoadMaterial(
        "Renderer2D_CircleMaterial", "Shaders/Renderer2D_CircleShader.vert", "Shaders/Renderer2D_CircleShader.frag");
    auto& circleModel = LoadModel("Renderer2D_Circle", circleMaterial, mesh);

    auto& fontMaterial =
        LoadMaterial("Renderer_FontMaterial", "Shaders/Renderer_FontShader.vert", "Shaders/Renderer_FontShader.frag");

    std::vector<glm::vec2> fontVertexBuffer = {{-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}};
    indexBuffer = {0, 1, 2, 2, 3, 0};

    auto& fontMesh = LoadMesh<glm::vec2>("Renderer_FontMesh", fontVertexBuffer, indexBuffer);
    auto& fontModel = LoadModel("Renderer_Font", fontMaterial, fontMesh);

    auto& lineMaterial =
        LoadMaterial("Renderer_LineMaterial", "Shaders/Renderer_LineShader.vert", "Shaders/Renderer_LineShader.frag");
    const float halfLineWidth = 0.5f;
    std::vector<glm::vec3> lineVertexBuffer = {
        {-0.5f, -halfLineWidth, 0.0f},
        {0.5f, -halfLineWidth, 0.0f},
        {0.5f, halfLineWidth, 0.0f},
        {-0.5f, halfLineWidth, 0.0f},
    };
    auto& lineMesh = LoadMesh<glm::vec3>("Renderer_LineMesh", lineVertexBuffer, indexBuffer);

    auto& lineModel = LoadModel("Renderer_Line", lineMaterial, lineMesh);

    auto& openSansRegularFont =
        LoadFont("OpenSansRegular", Internal::GetEmbeddedResource(EmbeddedResource::OpenSansRegular));

    std::vector<BeeEngine::Vertex> framebufferVertexBuffer = {{
                                                                  {-0.5f, -0.5f, 0.0f},
                                                                  {1.0f, 1.0f, 1.0f},
                                                                  {0.0f, 1.0f},
                                                              },
                                                              {
                                                                  {0.5f, -0.5f, 0.0f},
                                                                  {1.0f, 1.0f, 1.0f},
                                                                  {1.0f, 1.0f},
                                                              },
                                                              {
                                                                  {0.5f, 0.5f, 0.0f},
                                                                  {1.0f, 1.0f, 1.0f},
                                                                  {1.0f, 0.0f},
                                                              },
                                                              {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}};
    auto& framebufferMaterial = LoadMaterial(
        "Renderer_FramebufferMaterial", "Shaders/Renderer_Framebuffer.vert", "Shaders/Renderer_Framebuffer.frag");
    auto& framebufferMesh = LoadMesh("Renderer_FramebufferMesh", framebufferVertexBuffer, indexBuffer);
    auto& framebufferModel = LoadModel("Renderer_Framebuffer", framebufferMaterial, framebufferMesh);
    auto& defaultMeshMaterial = LoadMaterial("Renderer_DefaultMeshMaterial",
                                             "Shaders/Renderer_MeshDefaultShader.vert",
                                             "Shaders/Renderer_MeshDefaultShader.frag");
}

void BeeEngine::InternalAssetManager::CleanUp()
{
    m_Materials.clear();
    m_Meshes.clear();
    m_Models.clear();
    m_Textures.clear();
    m_Fonts.clear();
}
