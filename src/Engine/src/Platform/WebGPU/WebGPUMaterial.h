//
// Created by alexl on 15.07.2023.
//

#pragma once

#include "Renderer/Material.h"
#include "Renderer/Pipeline.h"
namespace BeeEngine::Internal
{
    class WebGPUMaterial final: public Material
    {
    public:
        WebGPUMaterial(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader, BindingSet* bindingSet, bool loadFromCache);
        ~WebGPUMaterial() override;
        [[nodiscard]] InstancedBuffer& GetInstancedBuffer() const override;

        void Bind(void* cmd) const
        {
            m_Pipeline->Bind(cmd);
        }
    private:
        Ref<Pipeline> m_Pipeline;
        Ref<InstancedBuffer> m_InstancedBuffer;
    };
}
