//
// Created by alexl on 15.07.2023.
//

#include "WebGPUMaterial.h"

namespace BeeEngine::Internal
{

    WebGPUMaterial::WebGPUMaterial(const std::filesystem::path &vertexShader,
                                   const std::filesystem::path &fragmentShader,
                                   BindingSet *bindingSet,
                                   bool loadFromCache)
    {
        auto vertexShaderModule = ShaderModule::Create(vertexShader.string(), ShaderType::Vertex, loadFromCache);
        auto fragmentShaderModule = ShaderModule::Create(fragmentShader.string(), ShaderType::Fragment, loadFromCache);
        m_InstancedBuffer = vertexShaderModule->CreateInstancedBuffer();
        m_Pipeline = Pipeline::Create(vertexShaderModule, fragmentShaderModule, bindingSet);
    }

    WebGPUMaterial::~WebGPUMaterial()
    {

    }

    InstancedBuffer &WebGPUMaterial::GetInstancedBuffer() const
    {
        return *m_InstancedBuffer;
    }
}