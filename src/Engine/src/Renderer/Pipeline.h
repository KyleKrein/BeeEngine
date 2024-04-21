//
// Created by Александр Лебедев on 01.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "ShaderModule.h"
#include "BindingSet.h"

namespace BeeEngine
{
    enum class PipelineType
    {
        Graphics,
        Compute
    };
    class Pipeline
    {
    public:
        virtual PipelineType GetType() const = 0;
        virtual void Bind(CommandBuffer& commandBuffer) = 0;
        virtual ~Pipeline() = default;
        [[nodiscard]] static Ref<Pipeline> Create(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader);
        [[nodiscard]] static Ref<Pipeline> Create(const Ref<ShaderModule>& computeShader);
    private:
    };
}