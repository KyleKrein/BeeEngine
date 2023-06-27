//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "Vertex.h"
#include "BufferLayout.h"

namespace BeeEngine
{
    class Mesh
    {
    public:
        Mesh() = default;
        virtual ~Mesh() = default;
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other ) = delete;
        static Ref<Mesh> Create(in<std::vector<Vertex>> vertices, BufferLayout layout);
    };
}