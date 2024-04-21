//
// Created by Aleksandr on 02.04.2024.
//

#include "MeshSource.h"

#include "Core/Application.h"

namespace BeeEngine
{
    MeshSource::MeshSource(std::unordered_map<String, Ref<Mesh>>&& meshes,
            std::unordered_map<String, Ref<Node>>&& nodes,
            std::unordered_map<String, Ref<Texture2D>>&& textures,
            std::unordered_map<String, Ref<MaterialInstance>>&& materials,
            std::vector<Ref<Node>>&& topNodes)
    :m_Meshes(std::move(meshes)), m_Nodes(std::move(nodes)), m_Textures(std::move(textures)), m_Materials(std::move(materials)), m_TopNodes(std::move(topNodes))
    {
        m_Models.reserve(m_Meshes.size());
        auto& meshDefaultMaterial = Application::GetInstance().GetAssetManager().GetMaterial("Renderer_DefaultMeshMaterial");
        for (auto& [_, mesh] : m_Meshes)
        {
            m_Models.emplace_back(*mesh, meshDefaultMaterial);
        }
    }

    Ref<MeshSource> MeshSourceBuilder::Build()
    {
        return CreateRef<MeshSource>(std::move(m_Meshes), std::move(m_Nodes), std::move(m_Textures), std::move(m_Materials), std::move(m_TopNodes));
    }
}
