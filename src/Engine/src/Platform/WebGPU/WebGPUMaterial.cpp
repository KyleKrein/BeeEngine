//
// Created by alexl on 15.07.2023.
//

#include "WebGPUMaterial.h"

namespace BeeEngine::Internal
{

    WebGPUMaterial::WebGPUMaterial(const std::filesystem::path& vertexShader,
                                   const std::filesystem::path& fragmentShader)
    {
        auto vertexShaderModule = ShaderModule::Create(String{vertexShader.string()}, ShaderType::Vertex);
        auto fragmentShaderModule = ShaderModule::Create(String{fragmentShader.string()}, ShaderType::Fragment);
        m_InstancedBuffer = vertexShaderModule->CreateInstancedBuffer();
        m_Pipeline = Pipeline::Create(vertexShaderModule, fragmentShaderModule);
    }

    WebGPUMaterial::~WebGPUMaterial() {}

    InstancedBuffer& WebGPUMaterial::GetInstancedBuffer() const
    {
        return *m_InstancedBuffer;
    }
} // namespace BeeEngine::Internal
