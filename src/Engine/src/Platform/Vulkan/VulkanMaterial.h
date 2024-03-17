//
// Created by Aleksandr on 09.03.2024.
//

#pragma once
#include "Renderer/Material.h"
#include "Renderer/Pipeline.h"

namespace BeeEngine::Internal
{

    class VulkanMaterial final: public Material
    {
    public:
        VulkanMaterial(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader, bool loadFromCache);
        ~VulkanMaterial() override;

        [[nodiscard]] InstancedBuffer& GetInstancedBuffer() const override;
        void Bind(void* cmd) override
        {
            m_Pipeline->Bind(cmd);
        }
    private:
        Ref<Pipeline> m_Pipeline;
        Ref<InstancedBuffer> m_InstancedBuffer;
    };

}
