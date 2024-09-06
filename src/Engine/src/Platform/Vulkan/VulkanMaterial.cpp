//
// Created by Aleksandr on 09.03.2024.
//

#include "VulkanMaterial.h"

namespace BeeEngine::Internal
{
    VulkanMaterial::VulkanMaterial(const std::filesystem::path& vertexShader,
                                   const std::filesystem::path& fragmentShader)
    {
        auto vertexShaderModule = ShaderModule::Create(vertexShader, ShaderType::Vertex);
        auto fragmentShaderModule = ShaderModule::Create(fragmentShader, ShaderType::Fragment);
        m_InstancedBuffer = vertexShaderModule->CreateInstancedBuffer();
        m_Pipeline = Pipeline::Create(vertexShaderModule, fragmentShaderModule);
    }

    VulkanMaterial::~VulkanMaterial() {}

    InstancedBuffer& VulkanMaterial::GetInstancedBuffer() const
    {
        return *m_InstancedBuffer;
    }
} // namespace BeeEngine::Internal
