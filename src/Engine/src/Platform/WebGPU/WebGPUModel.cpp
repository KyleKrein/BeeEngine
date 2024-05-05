//
// Created by alexl on 15.07.2023.
//

#include "Core/DeletionQueue.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "WebGPUMaterial.h"

namespace BeeEngine::Internal
{
    void BindModel(Mesh* mesh, Material* material, CommandBuffer& cmd)
    {
        static Material* lastMaterial = nullptr;
        if (lastMaterial != material)
        {
            lastMaterial = material;
            material->Bind(cmd);
        }
        static Mesh* lastMesh = nullptr;
        if (lastMesh != mesh)
        {
            lastMesh = mesh;
            mesh->Bind(cmd);
        }
        static bool isErased = false;
        if (!isErased)
        {
            isErased = true;
            DeletionQueue::RendererFlush().PushFunction(
                [&]()
                {
                    lastMaterial = nullptr;
                    lastMesh = nullptr;
                    isErased = false;
                });
        }
    }
} // namespace BeeEngine::Internal