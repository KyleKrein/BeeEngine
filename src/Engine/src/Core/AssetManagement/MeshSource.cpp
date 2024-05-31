//
// Created by Aleksandr on 02.04.2024.
//

#include "MeshSource.h"

#include "Core/Application.h"

namespace BeeEngine
{
    MeshSource::MeshSource(std::vector<Ref<Mesh>>&& meshes) : m_Meshes(std::move(meshes))
    {
        m_Models.reserve(m_Meshes.size());
        auto& meshDefaultMaterial =
            Application::GetInstance().GetAssetManager().GetMaterial("Renderer_DefaultMeshMaterial");
        for (auto& mesh : m_Meshes)
        {
            m_Models.emplace_back(*mesh, meshDefaultMaterial);
        }
    }
} // namespace BeeEngine
