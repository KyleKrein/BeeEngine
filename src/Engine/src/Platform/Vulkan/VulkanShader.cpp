//
// Created by alexl on 09.06.2023.
//

#include "VulkanShader.h"
#include "shaderc/shaderc.hpp"
#include "vulkan/vulkan.hpp"


namespace BeeEngine::Internal
{

    VulkanShader::VulkanShader(std::string_view name, std::string_view vertexShader, std::string_view fragmentShader)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        shaderc::SpvCompilationResult vertexShaderResult = compiler.CompileGlslToSpv(vertexShader.data(), vertexShader.size(), shaderc_glsl_vertex_shader, name.data(), options);
        shaderc::SpvCompilationResult fragmentShaderResult = compiler.CompileGlslToSpv(fragmentShader.data(), fragmentShader.size(), shaderc_glsl_fragment_shader, name.data(), options);

        if (vertexShaderResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            throw std::runtime_error(vertexShaderResult.GetErrorMessage());
        }
        if (fragmentShaderResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            throw std::runtime_error(fragmentShaderResult.GetErrorMessage());
        }
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
