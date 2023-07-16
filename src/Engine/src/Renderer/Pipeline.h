//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "ShaderModule.h"
#include "BindingSet.h"

namespace BeeEngine
{
    class Pipeline
    {
    public:
        virtual void Bind(void* commandBuffer) = 0;
        virtual ~Pipeline() = default;
        [[nodiscard]] static Ref<Pipeline> Create(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader);
    private:
    };
}