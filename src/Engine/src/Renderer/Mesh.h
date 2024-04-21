//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "Vertex.h"
#include "BufferLayout.h"
#include "CommandBuffer.h"
#include "MaterialData.h"
#include "Core/AssetManagement/Asset.h"

namespace BeeEngine
{
    struct MeshDefaultVertex
    {
        glm::vec3 position;
        float uv_x;
        glm::vec3 normal;
        float uv_y;
        glm::vec4 color;
    };
    struct GeoSurface
    {
        uint32_t startIndex;
        uint32_t count;
        Ref<MaterialInstance> material;
    };
    class Mesh: public Asset
    {
    public:
        constexpr AssetType GetType() const override
        {
            return AssetType::Mesh;
        }

        std::vector<GeoSurface> Surfaces;
        Mesh() = default;
        virtual ~Mesh() = default;
        [[nodiscard]] virtual uint32_t GetVertexCount() const = 0;
        [[nodiscard]] virtual uint32_t GetIndexCount() const = 0;
        virtual void Bind(CommandBuffer& commandBuffer) = 0;
        [[nodiscard]] virtual bool IsIndexed() const = 0;

        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other ) = delete;
        static Ref<Mesh> Create(in<std::vector<Vertex>> vertices);
        static Ref<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        static Ref<Mesh> Create(void* verticesData, size_t size, size_t vertexCount, const std::vector<uint32_t>& indices);
    };
}
