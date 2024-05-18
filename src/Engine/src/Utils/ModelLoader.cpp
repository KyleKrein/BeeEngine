//
// Created by Александр Лебедев on 26.06.2023.
//

#include "ModelLoader.h"
#include "tiny_obj_loader.h"

namespace BeeEngine::Internal
{

    bool ModelLoader::LoadObj(std::string_view path, out<std::vector<BeeEngine::Vertex>> vertices)
    {
        // attrib will contain the vertex arrays of the file
        tinyobj::attrib_t attrib;
        // shapes contains the info for each separate object in the file
        std::vector<tinyobj::shape_t> shapes;
        // materials contains the information about the material of each shape, but we won't use it.
        std::vector<tinyobj::material_t> materials;

        // error and warning output from the load function
        std::string warn;
        std::string err;

        // load the OBJ file
        tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.data(), nullptr);
        // make sure to output the warnings to the console, in case there are issues with the file
        if (!warn.empty())
        {
            BeeCoreWarn(String{warn});
        }
        // if we have any error, print it to the console, and break the mesh loading.
        // This happens if the file can't be found or is malformed
        if (!err.empty())
        {
            BeeCoreError(String{err});
            return false;
        }

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++)
        {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                // hardcode loading to triangles
                int fv = 3;

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++)
                {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    // vertex position
                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    // vertex normal
                    tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                    // copy it into our vertex
                    BeeEngine::Vertex new_vert{};
                    new_vert.Position.x = vx;
                    new_vert.Position.y = vy;
                    new_vert.Position.z = vz;

                    new_vert.Normal.x = nx;
                    new_vert.Normal.y = ny;
                    new_vert.Normal.z = nz;

                    // we are setting the vertex color as the vertex normal. This is just for display purposes
                    // new_vert.Color = new_vert.Normal;

                    vertices.push_back(new_vert);
                }
                index_offset += fv;
            }
        }

        return true;
    }

    bool ModelLoader::LoadglTF(std::string_view path,
                               out<std::vector<BeeEngine::Vertex>> vertices,
                               out<std::vector<uint32_t>> indices)
    {
        return false;
    }
} // namespace BeeEngine::Internal