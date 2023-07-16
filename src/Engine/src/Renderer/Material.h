//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "InstancedBuffer.h"
#include <filesystem>
#include "Renderer/BindingSet.h"

namespace BeeEngine
{
    class Material
    {
        public:
        Material() = default;
        virtual ~Material() = default;
        Material(const Material& other) = delete;
        Material& operator=(const Material& other ) = delete;

        [[nodiscard]] virtual InstancedBuffer& GetInstancedBuffer() const = 0;

        static Ref<Material> Create(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader, BindingSet* bindingSet, bool loadFromCache = true);
    };
}


