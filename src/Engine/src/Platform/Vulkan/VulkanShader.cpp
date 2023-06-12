//
// Created by alexl on 09.06.2023.
//

#include "VulkanShader.h"
#include "vulkan/vulkan.hpp"


namespace BeeEngine::Internal
{

    VulkanShader::VulkanShader(std::string_view name, std::string_view vertexShader, std::string_view fragmentShader)
    {

    }

    VulkanShader::VulkanShader(std::string_view name, gsl::span<std::byte> vertexShader,
                               gsl::span<std::byte> fragmentShader)
    {
        vk::ShaderModuleCreateInfo moduleInfo;
        moduleInfo.flags = vk::ShaderModuleCreateFlags();
        moduleInfo.codeSize = vertexShader.size();
        moduleInfo.pCode = (uint32_t*)vertexShader.data();

        try
        {
            //m_VertexShaderModule = m_Device.createShaderModule(moduleInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error(e.what());
        }
    }
}
