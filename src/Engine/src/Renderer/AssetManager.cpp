//
// Created by Александр Лебедев on 27.06.2023.
//

#include "AssetManager.h"
#include "Utils/ModelLoader.h"
#include "../../Assets/EmbeddedResources.h"
#include <glm.hpp>
#include "Vertex.h"

BeeEngine::Material &
BeeEngine::AssetManager::LoadMaterial(const std::string &name, const std::filesystem::path &vertexShader,
                                      const std::filesystem::path &fragmentShader)
{
    if(HasMaterial(name))
        return GetMaterial(name);
    else
        return *m_Materials.emplace(name, Material::Create(vertexShader, fragmentShader,
                                                           false //change to true when cache will be implemented
                                                           )).first->second;
}

BeeEngine::Mesh &BeeEngine::AssetManager::LoadMesh(const std::string &name, const std::filesystem::path &path)
{
    if(HasMesh(name))
        return GetMesh(name);
    else
    {
        std::vector<BeeEngine::Vertex> vertices;
        std::vector<uint32_t> indices;
        auto result = Internal::ModelLoader::LoadglTF(path.string(), vertices, indices);
        BeeEnsures(result);
        return *m_Meshes.emplace(name, Mesh::Create(vertices, indices)).first->second;
    }
}

BeeEngine::Mesh& BeeEngine::AssetManager::LoadMesh(const std::string& name, std::vector<BeeEngine::Vertex>& vertices, std::vector<uint32_t>& indices)
{
    if(HasMesh(name))
        return GetMesh(name);
    else
        return *m_Meshes.emplace(name, Mesh::Create(vertices, indices)).first->second;
}
template<typename VertexType>
BeeEngine::Mesh& BeeEngine::AssetManager::LoadMesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<uint32_t>& indices)
{
    if(HasMesh(name))
        return GetMesh(name);
    size_t vertexCount = vertices.size();
    size_t size = vertexCount * sizeof(VertexType);
    void* data = (void*)vertices.data();
    return *m_Meshes.emplace(name, Mesh::Create(data, size, vertexCount, indices)).first->second;
}

BeeEngine::Texture2D &BeeEngine::AssetManager::LoadTexture(const std::string &name, const std::filesystem::path &path)
{
    if (HasTexture(name))
    {
        return GetTexture(name);
    }
    return *m_Textures.emplace(name, Texture2D::Create(path.string())).first->second;
}

BeeEngine::Material &BeeEngine::AssetManager::GetMaterial(const std::string &name)
{
    BeeExpects(HasMaterial(name));
    return *m_Materials.at(name);
}

BeeEngine::Mesh &BeeEngine::AssetManager::GetMesh(const std::string &name)
{
    BeeExpects(HasMesh(name));
    return *m_Meshes.at(name);
}

BeeEngine::Texture2D &BeeEngine::AssetManager::GetTexture(const std::string &name)
{
    BeeExpects(HasTexture(name));
    return *m_Textures.at(name);
}

bool BeeEngine::AssetManager::HasMaterial(const std::string &name) const
{
    if(m_Materials.find(name) != m_Materials.end())
        return true;
    else
        return false;
}

bool BeeEngine::AssetManager::HasMesh(const std::string &name) const
{
    if (m_Meshes.find(name) != m_Meshes.end())
        return true;
    else
        return false;
}

bool BeeEngine::AssetManager::HasTexture(const std::string &name) const
{
    if (m_Textures.find(name) != m_Textures.end())
        return true;
    else
        return false;
}

BeeEngine::Model &
BeeEngine::AssetManager::LoadModel(const std::string &name, BeeEngine::Material &material, BeeEngine::Mesh &mesh)
{
    if(HasModel(name))
        return GetModel(name);
    else
        return *m_Models.emplace(name, Model::Load(mesh, material)).first->second;
}

BeeEngine::Model &BeeEngine::AssetManager::GetModel(const std::string &name)
{
    BeeExpects(HasModel(name));
    return *m_Models.at(name);
}

bool BeeEngine::AssetManager::HasModel(const std::string &name) const
{
    if(m_Models.find(name) != m_Models.end())
        return true;
    else
        return false;
}

BeeEngine::Texture2D&  BeeEngine::AssetManager::LoadTexture(const std::string& name, uint32_t width, uint32_t height)
{
    m_Textures.emplace(name, Texture2D::Create(width, height));
    return *m_Textures.at(name);
}

BeeEngine::Font &BeeEngine::AssetManager::LoadFont(const std::string &name, const std::filesystem::path &path)
{
    if(HasFont(name))
        return GetFont(name);
    else
        return *m_Fonts.emplace(name, CreateRef<Font>(path)).first->second;
}

BeeEngine::Font &BeeEngine::AssetManager::LoadFont(const std::string &name, gsl::span<byte> data)
{
    if(HasFont(name))
        return GetFont(name);
    else
        return *m_Fonts.emplace(name, CreateRef<Font>(name, data)).first->second;
}

BeeEngine::Font &BeeEngine::AssetManager::GetFont(const std::string &name)
{
    BeeExpects(HasFont(name));
    return *m_Fonts.at(name);
}

bool BeeEngine::AssetManager::HasFont(const std::string &name) const
{
    return m_Fonts.contains(name);
}

void BeeEngine::AssetManager::LoadStandardAssets()
{
    using namespace BeeEngine;
    auto& blank= LoadTexture("Blank", 1, 1);
    uint32_t data = 0xffffffff;
    blank.SetData({(byte *)&data, 4});

    auto& spriteMaterial = LoadMaterial("Renderer2D_SpriteMaterial", "Shaders/Renderer2D_SpriteShader.vert", "Shaders/Renderer2D_SpriteShader.frag");


    std::vector<BeeEngine::Vertex> vertexBuffer =
            {
                    {{-0.5f, -0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {0.0f, 0.0f}, },
                    {{0.5f, -0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {1.0f, 0.0f}, },
                    {{0.5f, 0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {1.0f, 1.0f}, },
                    {{-0.5f, 0.5f, 0.0f},  {1.0f, 1.0f, 1.0f},  {0.0f, 1.0f} }
            };
    std::vector<uint32_t> indexBuffer = {2, 1, 0, 0, 3, 2};
    auto& mesh = LoadMesh("Renderer2D_RectangleMesh", vertexBuffer, indexBuffer);
    auto& rectModel = LoadModel("Renderer2D_Rectangle", spriteMaterial, mesh);

    auto& circleMaterial = LoadMaterial("Renderer2D_CircleMaterial", "Shaders/Renderer2D_CircleShader.vert", "Shaders/Renderer2D_CircleShader.frag");
    auto& circleModel = LoadModel("Renderer2D_Circle", circleMaterial, mesh);

    auto& fontMaterial = LoadMaterial("Renderer_FontMaterial", "Shaders/Renderer_FontShader.vert", "Shaders/Renderer_FontShader.frag");

    std::vector<glm::vec2> fontVertexBuffer = {
            {-0.5f, -0.5f},
            {0.5f, -0.5f},
            {0.5f, 0.5f},
            {-0.5f, 0.5f}
    };

    auto& fontMesh = LoadMesh<glm::vec2>("Renderer_FontMesh", fontVertexBuffer, indexBuffer);
    auto& fontModel = LoadModel("Renderer_Font", fontMaterial, fontMesh);

    auto& openSansRegularFont = LoadFont("OpenSansRegular", Internal::GetEmbeddedResource(EmbeddedResource::OpenSansRegular));
}
