//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once
#include <unordered_map>
#include "Renderer/Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "ShaderModule.h"
#include "Model.h"
#include "Renderer/BindingSet.h"
#include "Font.h"

namespace BeeEngine
{
    class AssetManager final
    {
    public:
        [[nodiscard]] Material& LoadMaterial(const std::string& name, const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader);
        [[nodiscard]] Mesh& LoadMesh(const std::string& name, const std::filesystem::path& path);
        [[nodiscard]] Mesh& LoadMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        template<typename VertexType>
        [[nodiscard]] Mesh& LoadMesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<uint32_t>& indices);
        [[nodiscard]] Texture2D& LoadTexture(const std::string& name, const std::filesystem::path& path);
        [[nodiscard]] Texture2D& LoadTexture(const std::string& name, uint32_t width, uint32_t height);
        [[nodiscard]] Model& LoadModel(const std::string& name, Material& material, Mesh& mesh);
        [[nodiscard]] Font& LoadFont(const std::string& name, const std::filesystem::path& path);
        [[nodiscard]] Font& LoadFont(const std::string& name, gsl::span<byte> data);

        [[nodiscard]] Material& GetMaterial(const std::string& name);
        [[nodiscard]] Mesh& GetMesh(const std::string& name);
        [[nodiscard]] Texture2D& GetTexture(const std::string& name);
        [[nodiscard]] Model& GetModel(const std::string& name);
        [[nodiscard]] Font& GetFont(const std::string& name);

        [[nodiscard]] bool HasMaterial(const std::string& name) const;
        [[nodiscard]] bool HasMesh(const std::string& name) const;
        [[nodiscard]] bool HasTexture(const std::string& name) const;
        [[nodiscard]] bool HasModel(const std::string& name) const;
        [[nodiscard]] bool HasFont(const std::string& name) const;
    private:
        std::unordered_map<std::string, Ref<Material>> m_Materials;
        std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
        std::unordered_map<std::string, Ref<Model>> m_Models;
        std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
        std::unordered_map<std::string, Ref<Font>> m_Fonts;

        void LoadStandardAssets();
        friend class Application;
    };
}
