//
// Created by Александр Лебедев on 01.07.2023.
//

#include "WebGPUShaderModule.h"
#include "WebGPUGraphicsDevice.h"
#include "Utils/ShaderConverter.h"


namespace BeeEngine::Internal
{
    WebGPUShaderModule::WebGPUShaderModule(in<std::vector<uint32_t>> spirvCode, ShaderType type)
    : m_Type(type)
    {
        std::string wgsl;
        bool result = ShaderConverter::SPVtoWGSL(spirvCode, wgsl);
        BeeEnsures(result);
        WGPUShaderModuleWGSLDescriptor wgslDesc{};
        wgslDesc.source = wgsl.c_str();
        wgslDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        wgslDesc.chain.next = nullptr;

        /*WGPUShaderModuleSPIRVDescriptor spirvDesc{};
        spirvDesc.code = spirvCode.data();
        spirvDesc.codeSize = spirvCode.size();
        spirvDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        spirvDesc.chain.next = nullptr;*/

        WGPUShaderModuleDescriptor shaderDesc{};
        shaderDesc.nextInChain = &wgslDesc.chain;
        shaderDesc.label = "Shader Module";

        m_ShaderModule = wgpuDeviceCreateShaderModule(WebGPUGraphicsDevice::GetInstance().GetDevice(), &shaderDesc);
    }
    WebGPUShaderModule::~WebGPUShaderModule()
    {
        wgpuShaderModuleRelease(m_ShaderModule);
    }
}