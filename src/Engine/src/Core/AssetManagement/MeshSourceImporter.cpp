//
// Created by Aleksandr on 02.04.2024.
//

#include "MeshSourceImporter.h"
#include <fastgltf/core.hpp>

#include "fastgltf/tools.hpp"
#include <glm.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <gtx/quaternion.hpp>

#include "Core/Application.h"

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
        if(extension == ".gltf")
            return MeshSourceFormat::GLTF;
        if(extension == ".glb")
            return MeshSourceFormat::GLTF_BINARY;
        return MeshSourceFormat::Unknown;
    }
    Texture2D::Filter ExtractFilter(fastgltf::Filter filter)
    {
        switch (filter)
        {
            // nearest samplers
            case fastgltf::Filter::Nearest:
            case fastgltf::Filter::NearestMipMapNearest:
            case fastgltf::Filter::NearestMipMapLinear:
                return Texture2D::Filter::Nearest;
            // linear samplers
            case fastgltf::Filter::Linear:
            case fastgltf::Filter::LinearMipMapNearest:
            case fastgltf::Filter::LinearMipMapLinear:
            default:
                return Texture2D::Filter::Linear;
        }
    }
    Texture2D::Filter ExtractMimMapMode(fastgltf::Filter filter)
    {
        switch (filter)
        {
            case fastgltf::Filter::NearestMipMapNearest:
            case fastgltf::Filter::LinearMipMapNearest:
                return Texture2D::Filter::Nearest;
            case fastgltf::Filter::NearestMipMapLinear:
            case fastgltf::Filter::LinearMipMapLinear:
            default:
                return Texture2D::Filter::Linear;
        }
    }
    Ref<MeshSource> MeshSourceImporter::ImportMeshSource(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::MeshSource);
        BeeExpects(metadata.Location == AssetLocation::FileSystem);
        auto& path = std::get<Path>(metadata.Data);
        auto format = GetMeshSourceFormat(path);

        BeeCoreAssert(format != MeshSourceFormat::Unknown, "Unknown mesh source format for file: {0}", path);

        if(format == MeshSourceFormat::GLTF || format == MeshSourceFormat::GLTF_BINARY)
        {
            MeshSourceBuilder builder;

            fastgltf::Parser parser{};

            constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
            // fastgltf::Options::LoadExternalImages;

            fastgltf::GltfDataBuffer data;

            auto filepath = path.ToStdPath();
            data.loadFromFile(filepath);

            fastgltf::Asset gltf;

            auto type = fastgltf::determineGltfFileType(&data);
            if(type == fastgltf::GltfType::glTF)
            {
                auto load = parser.loadGltf(&data, filepath.parent_path(), gltfOptions);
                if(load)
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
                if(load)
                {
                    gltf = std::move(load.get());
                }
                else
                {
                    BeeCoreError("Failed to load glTF file: {0}. Error: {1}", filepath, load.error());
                    return nullptr;
                }
            }

            // load samplers
            std::vector<std::tuple<Texture2D::Filter, Texture2D::Filter, Texture2D::Filter>> samplers;
            for (fastgltf::Sampler& sampler : gltf.samplers) {
                samplers.emplace_back(ExtractFilter(sampler.minFilter.value_or(Texture2D::Filter::Nearest)), ExtractFilter(sampler.magFilter.value_or(Texture2D::Filter::Nearest)), ExtractMimMapMode(sampler.minFilter.value_or(Texture2D::Filter::Nearest)));
            }

            // load textures
            std::vector<Ref<Texture2D>> textures;
            for (fastgltf::Texture& texture : gltf.textures) {
                auto t = Application::GetInstance().GetAssetManager().GetTextureRef("Checkerboard");
                textures.push_back(t);
                builder.AddTexture(String{texture.name}, std::move(t));
            }

            std::vector<Ref<MaterialInstance>> materials;

            for(auto& mat : gltf.materials)
            {
                Ref<MaterialInstance> newMat = CreateRef<MaterialInstance>();
                materials.push_back(newMat);
                //newMat->Name = mat.name;
                newMat->data.colorFactors.x = mat.pbrData.baseColorFactor[0];
                newMat->data.colorFactors.y = mat.pbrData.baseColorFactor[1];
                newMat->data.colorFactors.z = mat.pbrData.baseColorFactor[2];
                newMat->data.colorFactors.w = mat.pbrData.baseColorFactor[3];

                newMat->data.metalRoughFactors.x = mat.pbrData.metallicFactor;
                newMat->data.metalRoughFactors.y = mat.pbrData.roughnessFactor;

                /*MaterialPass passType = MaterialPass::MainColor;
                if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
                    passType = MaterialPass::Transparent;
                }*/

                if(mat.pbrData.baseColorTexture.has_value())
                {
                    size_t img = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
                    size_t sampler = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

                    newMat->colorTextureRef = textures[img];
                }

                newMat->LoadData();
                newMat->RebuildBindingSet();

                builder.AddMaterial(String{mat.name}, std::move(newMat));
            }

            std::vector<Ref<Mesh>> meshes;

            std::vector<uint32_t> indices;
            std::vector<MeshDefaultVertex> vertices;

            for(fastgltf::Mesh& mesh: gltf.meshes)
            {
                // clear the mesh arrays each mesh, we dont want to merge them by error
                indices.clear();
                vertices.clear();

                std::vector<GeoSurface> surfaces;

                for(auto&& p : mesh.primitives)
                {
                    GeoSurface newSurface;
                    newSurface.startIndex = indices.size();
                    newSurface.count = gltf.accessors[p.indicesAccessor.value()].count;

                    size_t initialVtx = vertices.size();

                    //load indexes
                    {
                        fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];
                        indices.reserve(indices.size() + indexAccessor.count);

                        fastgltf::iterateAccessor<uint32_t>(gltf, indexAccessor, [initialVtx, &indices](uint32_t index)
                        {
                            indices.push_back(index + initialVtx);
                        });
                    }

                    //load vertex positions
                    {
                        fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                        vertices.resize(vertices.size() + posAccessor.count);

                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                        [&vertices, initialVtx](glm::vec3 v, size_t index) {
                            MeshDefaultVertex newvtx;
                            newvtx.position = v;
                            newvtx.normal = { 1, 0, 0 };
                            newvtx.color = glm::vec4 { 1.f };
                            newvtx.uv_x = 0;
                            newvtx.uv_y = 0;
                            vertices[initialVtx + index] = newvtx;
                        });
                    }

                    //load vertex normals
                    auto normals = p.findAttribute("NORMAL");
                    if (normals != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second],
                            [&](glm::vec3 v, size_t index) {
                                vertices[initialVtx + index].normal = v;
                            });
                    }

                    // load UVs
                    auto uv = p.findAttribute("TEXCOORD_0");
                    if (uv != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second],
                            [&](glm::vec2 v, size_t index) {
                                vertices[initialVtx + index].uv_x = v.x;
                                vertices[initialVtx + index].uv_y = v.y;
                            });
                    }

                    // load vertex colors
                    auto colors = p.findAttribute("COLOR_0");
                    if (colors != p.attributes.end()) {

                        fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second],
                            [&](glm::vec4 v, size_t index) {
                                vertices[initialVtx + index].color = v;
                            });
                    }

                    if (p.materialIndex.has_value()) {
                        newSurface.material = materials[p.materialIndex.value()];
                    } else {
                        newSurface.material = materials[0];
                    }

                    surfaces.push_back(newSurface);
                }

                // display the vertex normals
                constexpr bool OverrideColors = false;
                if (OverrideColors) {
                    for (auto& vtx : vertices) {
                        vtx.color = glm::vec4(vtx.normal, 1.f);
                    }
                }
                Ref<Mesh> newMesh = Mesh::Create(vertices.data(), vertices.size() * sizeof(MeshDefaultVertex), vertices.size(), indices);
                newMesh->Surfaces = std::move(surfaces);
                newMesh->Name = mesh.name;
                //newMesh->Location = AssetLocation::MeshSource;
                meshes.emplace_back(std::move(newMesh));
            }

            std::vector<Ref<Node>> nodes;
            // load all nodes and their meshes
            for (fastgltf::Node& node : gltf.nodes) {
                std::shared_ptr<Node> newNode;

                // find if the node has a mesh, and if it does hook it to the mesh pointer and allocate it with the meshnode class
                if (node.meshIndex.has_value()) {
                    newNode = std::make_shared<MeshNode>();
                    static_cast<MeshNode*>(newNode.get())->Mesh = meshes[*node.meshIndex];
                } else {
                    newNode = std::make_shared<Node>();
                }

                nodes.push_back(newNode);
                builder.AddNode(String{node.name}, Ref{newNode});

                std::visit(fastgltf::visitor { [&](fastgltf::Node::TransformMatrix matrix) {
                                                  memcpy(&newNode->LocalTransform, matrix.data(), sizeof(matrix));
                                              },
                               [&](fastgltf::TRS transform) {
                                   glm::vec3 tl(transform.translation[0], transform.translation[1],
                                       transform.translation[2]);
                                   glm::quat rot(transform.rotation[3], transform.rotation[0], transform.rotation[1],
                                       transform.rotation[2]);
                                   glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);

                                   glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
                                   glm::mat4 rm = glm::toMat4(rot);
                                   glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

                                   newNode->LocalTransform = tm * rm * sm;
                               } },
                    node.transform);
            }
            // run loop again to setup transform hierarchy
            for (int i = 0; i < gltf.nodes.size(); i++) {
                fastgltf::Node& node = gltf.nodes[i];
                std::shared_ptr<Node>& sceneNode = nodes[i];

                for (auto& c : node.children) {
                    sceneNode->Children.push_back(nodes[c]);
                    nodes[c]->Parent = sceneNode;
                }
            }

            // find the top nodes, with no parents
            for (auto& node : nodes) {
                if (node->Parent.lock() == nullptr) {
                    builder.AddTopNode(node);
                    node->RefreshTransform(glm::mat4 { 1.f });
                }
            }
            auto result = builder.Build();
            result->Handle = handle;
            result->Name = metadata.Name;
            return result;
        }
        BeeCoreError("Unable to import mesh source from file: {0}", path);
        return nullptr;
    }
}
