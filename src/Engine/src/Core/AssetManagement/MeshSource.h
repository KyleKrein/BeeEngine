//
// Created by Aleksandr on 02.04.2024.
//

#pragma once
#include "Asset.h"
#include "Core/TypeDefines.h"
#include "Renderer/MaterialData.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"

namespace BeeEngine
{
    class MeshSourceBuilder;
    struct Node
    {
        WeakRef<Node> Parent;
        std::vector<Ref<Node>> Children;

        glm::mat4 WorldTransform = glm::mat4(1.0f);
        glm::mat4 LocalTransform = glm::mat4(1.0f);

        void RefreshTransform(const glm::mat4& parentMatrix)
        {
            WorldTransform = parentMatrix * LocalTransform;
            for (auto& c : Children) {
                c->RefreshTransform(WorldTransform);
            }
        }
    };
    struct MeshNode: public Node
    {
        Ref<Mesh> Mesh;
    };
    class MeshSource final: public Asset
    {
    public:
        MeshSource(std::unordered_map<String, Ref<Mesh>>&& meshes,
            std::unordered_map<String, Ref<Node>>&& nodes,
            std::unordered_map<String, Ref<Texture2D>>&& textures,
            std::unordered_map<String, Ref<MaterialInstance>>&& materials,
            std::vector<Ref<Node>>&& topNodes);

        constexpr AssetType GetType() const override
        {
            return AssetType::MeshSource;
        }

        [[nodiscard]] auto& GetModels()
        {
            return m_Models;
        }
    private:
        std::unordered_map<String, Ref<Mesh>> m_Meshes;
        std::unordered_map<String, Ref<Node>> m_Nodes;
        std::unordered_map<String, Ref<Texture2D>> m_Textures;
        std::unordered_map<String, Ref<MaterialInstance>> m_Materials;
        // nodes that dont have a parent, for iterating through the file in tree order
        std::vector<Ref<Node>> m_TopNodes;
        std::vector<Model> m_Models;
    };
    class MeshSourceBuilder
    {
    public:
        MeshSourceBuilder() = default;

        void AddMesh(String&& name, Ref<Mesh>&& mesh)
        {
            m_Meshes[std::move(name)] = std::move(mesh);
        }

        void AddNode(String&& name, Ref<Node>&& node)
        {
            m_Nodes[std::move(name)] = std::move(node);
        }

        void AddTexture(String&& name, Ref<Texture2D>&& texture)
        {
            m_Textures[std::move(name)] = std::move(texture);
        }

        void AddMaterial(String&& name, Ref<MaterialInstance>&& material)
        {
            m_Materials[std::move(name)] = std::move(material);
        }

        void AddTopNode(Ref<Node> node)
        {
            m_TopNodes.push_back(std::move(node));
        }

        Ref<MeshSource> Build();
    private:
        std::unordered_map<String, Ref<Mesh>> m_Meshes;
        std::unordered_map<String, Ref<Node>> m_Nodes;
        std::unordered_map<String, Ref<Texture2D>> m_Textures;
        std::unordered_map<String, Ref<MaterialInstance>> m_Materials;
        std::vector<Ref<Node>> m_TopNodes;
    };
}
