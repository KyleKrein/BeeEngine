//
// Created by Aleksandr on 09.03.2024.
//

#include "VulkanMaterial.h"


namespace BeeEngine::Internal
{
    VulkanMaterial::VulkanMaterial(const std::filesystem::path& vertexShader,
        const std::filesystem::path& fragmentShader, bool loadFromCache)
    {
        auto vertexShaderModule = ShaderModule::Create(vertexShader.string(), ShaderType::Vertex, loadFromCache);
        auto fragmentShaderModule = ShaderModule::Create(fragmentShader.string(), ShaderType::Fragment, loadFromCache);
        m_InstancedBuffer = vertexShaderModule->CreateInstancedBuffer();
        m_Pipeline = Pipeline::Create(vertexShaderModule, fragmentShaderModule);
    }

    VulkanMaterial::~VulkanMaterial()
    {
    }

    InstancedBuffer& VulkanMaterial::GetInstancedBuffer() const
    {
        return *m_InstancedBuffer;
    }
}
