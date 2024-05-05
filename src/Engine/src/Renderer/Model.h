//
// Created by alexl on 15.07.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include "Material.h"
#include "Mesh.h"

namespace BeeEngine
{
    class Model final
    {
    public:
        [[nodiscard]] Mesh& GetMesh() { return *m_Mesh; }

        [[nodiscard]] Material& GetMaterial() { return *m_Material; }

        virtual ~Model() = default;
        // Model& operator=(const Model& other) = delete;
        // Model(const Model& other) = delete;
        // Model& operator=(Model&& other) = delete;
        // Model(Model&& other) = delete;

        void Bind(CommandBuffer& commandBuffer) const;

        [[nodiscard]] bool IsIndexed() const { return m_Mesh->IsIndexed(); }
        [[nodiscard]] uint32_t GetVertexCount() const { return m_Mesh->GetVertexCount(); }
        [[nodiscard]] uint32_t GetIndexCount() const { return m_Mesh->GetIndexCount(); }

        [[nodiscard]] static Ref<Model> Load(Mesh& mesh, Material& material);
        Model(Mesh& mesh, Material& material) : m_Mesh(&mesh), m_Material(&material) {}

    private:
        Model() = default;

    private:
        Mesh* m_Mesh;
        Material* m_Material;
    };
} // namespace BeeEngine