//
// Created by alexl on 09.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_VULKAN)
#include <gsl/span>
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanShaderModule
    {
    public:
        VulkanShaderModule(std::string_view name, gsl::span<std::byte> shaderCode);
        VulkanShaderModule(std::string_view name, std::string_view filepath);
        VulkanShaderModule(vk::Device& device, std::string_view name, std::string_view filepath);
        ~VulkanShaderModule();

        vk::ShaderModule& GetHandle()
        {
            return m_ShaderModule;
        }
    private:
        vk::ShaderModule m_ShaderModule;
        vk::Device& m_Device;

        void CreateShaderModule(gsl::span<std::byte> shaderCode);
    };
}
#endif