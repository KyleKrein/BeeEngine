//
// Created by alexl on 15.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
namespace BeeEngine
{
    class IBindable
    {
        public:
            virtual ~IBindable() = default;
            virtual void Bind(uint32_t slot = 0) = 0;
            virtual std::vector<struct WGPUBindGroupLayoutEntry> GetBindGroupLayoutEntry() const = 0;
            virtual std::vector<struct WGPUBindGroupEntry> GetBindGroupEntry() const = 0;
    };
}