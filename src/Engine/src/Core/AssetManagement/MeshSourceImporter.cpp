//
// Created by Aleksandr on 02.04.2024.
//

#include "MeshSourceImporter.h"
#include <fastgltf/core.hpp>

#include "fastgltf/tools.hpp"
#include <fastgltf/glm_element_traits.hpp>

namespace BeeEngine
{
    enum class MeshSourceFormat
    {
        Unknown,
        GLTF,
        GLTF_BINARY,
    };
    MeshSourceFormat GetMeshSourceFormat(const Path& path)
    {
        auto extension = path.GetExtension();
        if (extension == ".gltf")
            return MeshSourceFormat::GLTF;
        if (extension == ".glb")
            return MeshSourceFormat::GLTF_BINARY;
        return MeshSourceFormat::Unknown;
    }
    Ref<MeshSource> MeshSourceImporter::ImportMeshSource(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::MeshSource);
        BeeExpects(metadata.Location == AssetLocation::FileSystem);
        auto& path = std::get<Path>(metadata.Data);
        auto format = GetMeshSourceFormat(path);

        BeeCoreAssert(format != MeshSourceFormat::Unknown, "Unknown mesh source format for file: {0}", path);

        if (format == MeshSourceFormat::GLTF || format == MeshSourceFormat::GLTF_BINARY)
        {
            fastgltf::Parser parser{};

            constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember |
                                         fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers |
                                         fastgltf::Options::LoadExternalBuffers;

            fastgltf::GltfDataBuffer data;

            auto filepath = path.ToStdPath();
            data.loadFromFile(filepath);

            fastgltf::Asset gltf;

            auto type = fastgltf::determineGltfFileType(&data);
            if (type == fastgltf::GltfType::glTF)
            {
                auto load = parser.loadGltf(&data, filepath.parent_path(), gltfOptions);
                if (load)
                {
                    gltf = std::move(load.get());
                }
                else
                {
                    BeeCoreError("Failed to load glTF file: {0}. Error: {1}", filepath, load.error());
                    return nullptr;
                }
            }
            else if (type == fastgltf::GltfType::GLB)
            {
                auto load = parser.loadGltfBinary(&data, filepath.parent_path(), gltfOptions);
                if (load)
                {
                    gltf = std::move(load.get());
                }
                else
                {
                    BeeCoreError("Failed to load glTF file: {0}. Error: {1}", filepath, load.error());
                    return nullptr;
                }
            }
            /*//load samplers
            for(fastgltf::Sampler& sampler : gltf.samplers)
            {

            }*/
            std::vector<Ref<Mesh>> meshes;

            std::vector<uint32_t> indices;
            std::vector<MeshDefaultVertex> vertices;

            for (fastgltf::Mesh& mesh : gltf.meshes)
            {
                // clear the mesh arrays each mesh, we dont want to merge them by error
                indices.clear();
                vertices.clear();

                std::vector<GeoSurface> surfaces;

                for (auto&& p : mesh.primitives)
                {
                    GeoSurface newSurface;
                    newSurface.startIndex = indices.size();
                    newSurface.count = gltf.accessors[p.indicesAccessor.value()].count;

                    size_t initialVtx = vertices.size();

                    // load indexes
                    {
                        fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];
                        indices.reserve(indices.size() + indexAccessor.count);

                        fastgltf::iterateAccessor<uint32_t>(gltf,
                                                            indexAccessor,
                                                            [initialVtx, &indices](uint32_t index)
                                                            { indices.push_back(index + initialVtx); });
                    }

                    // load vertex positions
                    {
                        fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                        vertices.resize(vertices.size() + posAccessor.count);

                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf,
                                                                      posAccessor,
                                                                      [&vertices, initialVtx](glm::vec3 v, size_t index)
                                                                      {
                                                                          MeshDefaultVertex newvtx;
                                                                          newvtx.position = v;
                                                                          newvtx.normal = {1, 0, 0};
                                                                          newvtx.color = glm::vec4{1.f};
                                                                          newvtx.uv_x = 0;
                                                                          newvtx.uv_y = 0;
                                                                          vertices[initialVtx + index] = newvtx;
                                                                      });
                    }

                    // load vertex normals
                    auto normals = p.findAttribute("NORMAL");
                    if (normals != p.attributes.end())
                    {
                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf,
                                                                      gltf.accessors[(*normals).second],
                                                                      [&](glm::vec3 v, size_t index)
                                                                      { vertices[initialVtx + index].normal = v; });
                    }

                    // load UVs
                    auto uv = p.findAttribute("TEXCOORD_0");
                    if (uv != p.attributes.end())
                    {
                        fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf,
                                                                      gltf.accessors[(*uv).second],
                                                                      [&](glm::vec2 v, size_t index)
                                                                      {
                                                                          vertices[initialVtx + index].uv_x = v.x;
                                                                          vertices[initialVtx + index].uv_y = v.y;
                                                                      });
                    }

                    // load vertex colors
                    auto colors = p.findAttribute("COLOR_0");
                    if (colors != p.attributes.end())
                    {
                        fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf,
                                                                      gltf.accessors[(*colors).second],
                                                                      [&](glm::vec4 v, size_t index)
                                                                      { vertices[initialVtx + index].color = v; });
                    }

                    surfaces.push_back(newSurface);
                }

                // display the vertex normals
                constexpr bool OverrideColors = true;
                if (OverrideColors)
                {
                    for (auto& vtx : vertices)
                    {
                        vtx.color = glm::vec4(vtx.normal, 1.f);
                    }
                }
                Ref<Mesh> newMesh = Mesh::Create(
                    vertices.data(), vertices.size() * sizeof(MeshDefaultVertex), vertices.size(), indices);
                newMesh->Surfaces = std::move(surfaces);
                newMesh->Name = mesh.name;
                // newMesh->Location = AssetLocation::MeshSource;
                meshes.emplace_back(std::move(newMesh));
            }
            auto result = CreateRef<MeshSource>(std::move(meshes));
            result->Handle = handle;
            result->Name = std::string_view{metadata.Name};
            return result;
        }
        BeeCoreError("Unable to import mesh source from file: {0}", path);
        return nullptr;
    }
} // namespace BeeEngine
