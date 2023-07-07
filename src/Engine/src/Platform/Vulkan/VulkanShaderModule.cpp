//
// Created by alexl on 09.06.2023.
//

#include "VulkanShaderModule.h"
#include "Utils/File.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "VulkanGraphicsDevice.h"


namespace BeeEngine::Internal
{

    VulkanShaderModule::VulkanShaderModule(std::string_view name, gsl::span<std::byte> shaderCode)
    : m_Device((*(VulkanGraphicsDevice*)&(WindowHandler::GetInstance()->GetGraphicsDevice())).GetDevice())
    {
        CreateShaderModule(shaderCode);
    }

    VulkanShaderModule::VulkanShaderModule(std::string_view name, std::string_view filepath)
    : m_Device((*(VulkanGraphicsDevice*)&(WindowHandler::GetInstance()->GetGraphicsDevice())).GetDevice())
    {
        std::vector<std::byte> shaderCode = File::ReadBinaryFile(filepath);
        CreateShaderModule(shaderCode);

    }

    void VulkanShaderModule::CreateShaderModule(gsl::span<std::byte> shaderCode)
    {
        vk::ShaderModuleCreateInfo moduleCreateInfo;
        moduleCreateInfo.flags = vk::ShaderModuleCreateFlags();
        moduleCreateInfo.codeSize = shaderCode.size();
        moduleCreateInfo.pCode = (uint32_t*)shaderCode.data();

        try
        {
            m_ShaderModule = m_Device.createShaderModule(moduleCreateInfo);
        }
        catch (vk::SystemError& e)
        {
            throw std::runtime_error(e.what());
        }
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        m_Device.destroyShaderModule(m_ShaderModule);
    }

    VulkanShaderModule::VulkanShaderModule(vk::Device &device, std::string_view name, std::string_view filepath)
    : m_Device(device)
    {
        std::vector<std::byte> shaderCode = File::ReadBinaryFile(filepath);
        CreateShaderModule(shaderCode);
    }
}
