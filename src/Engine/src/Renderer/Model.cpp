//
// Created by alexl on 15.07.2023.
//

#include "Model.h"

namespace BeeEngine
{
    namespace Internal
    {
        void WebGPUBindModel(Mesh* mesh, Material* material);
    }
    Ref<Model> BeeEngine::Model::Load(Mesh &mesh, Material &material)
    {
        return CreateRef<Model>(mesh, material);
    }

    void Model::Bind() const
    {
        Internal::WebGPUBindModel(m_Mesh, m_Material);
    }
}