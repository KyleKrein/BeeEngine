//
// Created by Aleksandr on 02.04.2024.
//

#pragma once
#include <glm/glm.hpp>

#include "Texture.h"
#include "UniformBuffer.h"

namespace BeeEngine
{
    struct MaterialData
    {
        glm::vec4 colorFactors = glm::vec4(1.0f);
        glm::vec4 metalRoughFactors; // x - metalness, y - roughness
    };

    struct MaterialInstance
    {
        MaterialData data;
        AssetHandle colorTexture;
        AssetHandle metalRoughTexture;
        Ref<UniformBuffer> dataBuffer = UniformBuffer::Create(sizeof(MaterialData));
        Ref<BindingSet> bindingSet =
            BindingSet::Create({{0, *dataBuffer}, {1, *GetColorTexture()}, {3, *GetMetalRoughTexture()}});

        [[nodiscard]] Texture2D* GetColorTexture() const;
        [[nodiscard]] Texture2D* GetMetalRoughTexture() const;
        void RebuildBindingSet()
        {
            bindingSet = BindingSet::Create({{0, *dataBuffer}, {1, *GetColorTexture()}, {3, *GetMetalRoughTexture()}});
        }
        void LoadData()
        {
            if (dataBuffer == nullptr)
            {
                dataBuffer = UniformBuffer::Create(sizeof(MaterialData));
                bindingSet =
                    BindingSet::Create({{0, *dataBuffer}, {1, *GetColorTexture()}, {3, *GetMetalRoughTexture()}});
            }
            dataBuffer->SetData(&data, sizeof(MaterialData));
        }
    };
} // namespace BeeEngine
