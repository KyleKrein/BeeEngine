//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/Shader.h"

namespace BeeEngine::Internal
{
    class VulkanShader: Shader
    {
    public:
        VulkanShader(std::string_view name ,std::string_view vertexShader, std::string_view fragmentShader);
        VulkanShader(std::string_view name, gsl::span<std::byte> vertexShader, gsl::span<std::byte> fragmentShader);
        ~VulkanShader() override;

    };
}
