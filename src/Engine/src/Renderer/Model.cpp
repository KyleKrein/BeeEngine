//
// Created by alexl on 15.07.2023.
//

#include "Model.h"

#include "Renderer.h"

namespace BeeEngine
{
    namespace Internal
    {
        void BindModel(Mesh* mesh, Material* material);
    }
    Ref<Model> BeeEngine::Model::Load(Mesh &mesh, Material &material)
    {
        return CreateRef<Model>(mesh, material);
    }

    void Model::Bind() const
    {
        /*switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                Internal::WebGPUBindModel(m_Mesh, m_Material);
                break;
#endif
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:*/
                Internal::BindModel(m_Mesh, m_Material);
                /*break;
#endif

        }*/
    }
}
