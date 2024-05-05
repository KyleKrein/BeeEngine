//
// Created by Александр Лебедев on 26.06.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "Renderer/Vertex.h"

namespace BeeEngine::Internal
{
    class ModelLoader
    {
    public:
        [[nodiscard("Returns if loading was successful")]] static bool
        LoadObj(std::string_view path, out<std::vector<BeeEngine::Vertex>> vertices);
        [[nodiscard("Returns if loading was successful")]] static bool LoadglTF(
            std::string_view path, out<std::vector<BeeEngine::Vertex>> vertices, out<std::vector<uint32_t>> indices);
    };
} // namespace BeeEngine::Internal
