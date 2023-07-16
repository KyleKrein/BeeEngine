//
// Created by Александр Лебедев on 27.06.2023.
//

#include "AssetManager.h"
#include "Utils/ModelLoader.h"

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

BeeEngine::Texture2D &BeeEngine::AssetManager::LoadTexture(const std::string &name, const std::filesystem::path &path)
{
    if (HasTexture(name))
        return GetTexture(name);
    else
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
    if (m_Meshes.find(name) != m_Meshes.end())
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
