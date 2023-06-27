//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class Material
    {
        public:
        Material() = default;
        virtual ~Material() = default;
        Material(const Material& other) = delete;
        Material& operator=(const Material& other ) = delete;

        static Ref<Material> Create();
    };
}


