//
// Created by alexl on 15.07.2023.
//

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "WebGPUMaterial.h"
#include "Core/DeletionQueue.h"

namespace BeeEngine::Internal
{
    void WebGPUBindModel(Mesh* mesh, Material* material)
    {
        auto cmd = Renderer::GetCurrentRenderPass();
        static Material* lastMaterial = nullptr;
        if (lastMaterial != material)
        {
            lastMaterial = material;
            ((WebGPUMaterial*)material)->Bind(&cmd);
        }
        static Mesh* lastMesh = nullptr;
        if (lastMesh != mesh)
        {
            lastMesh = mesh;
            mesh->Bind(&cmd);
        }
        static bool isErased = false;
        if (!isErased)
        {
            isErased = true;
            DeletionQueue::RendererFlush().PushFunction([&]()
            {
                lastMaterial = nullptr;
                lastMesh = nullptr;
                isErased = false;
            });
        }
    }
}