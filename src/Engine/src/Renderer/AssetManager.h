//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once
#include "Font.h"
#include "Mesh.h"
#include "Model.h"
#include "Renderer/BindingSet.h"
#include "Renderer/Material.h"
#include "ShaderModule.h"
#include "Texture.h"
#include <unordered_map>

namespace BeeEngine
{
    class InternalAssetManager final
    {
    public:
        [[nodiscard]] Material& LoadMaterial(const String& name,
                                             const std::filesystem::path& vertexShader,
                                             const std::filesystem::path& fragmentShader);
        [[nodiscard]] Mesh& LoadMesh(const String& name, const std::filesystem::path& path);
        [[nodiscard]] Mesh& LoadMesh(const String& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        template <typename VertexType>
        [[nodiscard]] Mesh&
        LoadMesh(const String& name, std::vector<VertexType>& vertices, std::vector<uint32_t>& indices);
        [[nodiscard]] Texture2D& LoadTexture(const String& name, const std::filesystem::path& path);
        [[nodiscard]] Texture2D& LoadTexture(const String& name, uint32_t width, uint32_t height, std::span<byte> data);
        [[nodiscard]] Model& LoadModel(const String& name, Material& material, Mesh& mesh);
        [[nodiscard]] Font& LoadFont(const String& name, const std::filesystem::path& path);
        [[nodiscard]] Font& LoadFont(const String& name, std::span<byte> data);

        [[nodiscard]] Material& GetMaterial(const String& name);
        [[nodiscard]] Mesh& GetMesh(const String& name);
        [[nodiscard]] Texture2D& GetTexture(const String& name);
        [[nodiscard]] Model& GetModel(const String& name);
        [[nodiscard]] Font& GetFont(const String& name);

        [[nodiscard]] bool HasMaterial(const String& name) const;
        [[nodiscard]] bool HasMesh(const String& name) const;
        [[nodiscard]] bool HasTexture(const String& name) const;
        [[nodiscard]] bool HasModel(const String& name) const;
        [[nodiscard]] bool HasFont(const String& name) const;

    private:
        std::unordered_map<String, Ref<Material>> m_Materials;
        std::unordered_map<String, Ref<Mesh>> m_Meshes;
        std::unordered_map<String, Ref<Model>> m_Models;
        std::unordered_map<String, Ref<Texture2D>> m_Textures;
        std::unordered_map<String, Ref<Font>> m_Fonts;

        void LoadStandardAssets();
        void CleanUp();
        friend class Application;
    };
} // namespace BeeEngine
