//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "InstancedBuffer.h"
#include "Renderer/BindingSet.h"
#include <filesystem>

namespace BeeEngine
{
    class CommandBuffer;
}

namespace BeeEngine
{
    class Material
    {
    public:
        Material() = default;
        virtual ~Material() = default;
        Material(const Material& other) = delete;
        Material& operator=(const Material& other) = delete;

        [[nodiscard]] virtual InstancedBuffer& GetInstancedBuffer() const = 0;

        virtual void Bind(CommandBuffer& cmd) = 0;

        static Ref<Material> Create(const std::filesystem::path& vertexShader,
                                    const std::filesystem::path& fragmentShader,
                                    bool loadFromCache = true);
    };
} // namespace BeeEngine
