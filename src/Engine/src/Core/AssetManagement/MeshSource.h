//
// Created by Aleksandr on 02.04.2024.
//

#pragma once
#include "Asset.h"
#include "Core/TypeDefines.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"

namespace BeeEngine
{
    class MeshSource final : public Asset
    {
    public:
        MeshSource(std::vector<Ref<Mesh>>&& meshes);

        constexpr AssetType GetType() const override { return AssetType::MeshSource; }

        [[nodiscard]] auto& GetModels() { return m_Models; }

    private:
        std::vector<Ref<Mesh>> m_Meshes;
        std::vector<Model> m_Models;
    };
} // namespace BeeEngine
