//
// Created by alexl on 15.07.2023.
//

#pragma once
#include <variant>
#if defined(BEE_COMPILE_WEBGPU)
#include <dawn/webgpu.h>
#endif
#if defined(BEE_COMPILE_VULKAN)
#include <vulkan/vulkan.hpp>
#endif

#include "Core/TypeDefines.h"
namespace BeeEngine
{
    class IBindable
    {
        public:
        struct Dummy {};
        using BindGroupLayoutEntryType = std::variant<
#if defined(BEE_COMPILE_WEBGPU)
            WGPUBindGroupLayoutEntry,
#endif
#if defined(BEE_COMPILE_VULKAN)
            vk::DescriptorSetLayoutBinding,
#endif
            Dummy
            >;
        using BindGroupEntryType = std::variant<
#if defined(BEE_COMPILE_WEBGPU)
            WGPUBindGroupEntry,
#endif
#if defined(BEE_COMPILE_VULKAN)
            vk::WriteDescriptorSet,
#endif
            Dummy>;
            virtual ~IBindable() = default;
            virtual void Bind(uint32_t slot = 0) = 0;
            virtual std::vector<BindGroupLayoutEntryType> GetBindGroupLayoutEntry() const = 0;
            virtual std::vector<BindGroupEntryType> GetBindGroupEntry() const = 0;
    };
}