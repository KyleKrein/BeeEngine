//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Shader.h"
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class ShaderLibrary
    {
    public:
        void Add(const Ref<Shader>& shader);
        void Add(std::string_view name, const Ref<Shader>& shader);
        Ref<Shader> Load(std::string_view filepath);
        Ref<Shader> Load(std::string_view name, std::string_view filepath);

        Ref<Shader> Get(std::string_view name) const;
        bool Exists(std::string_view name) const;

        static ShaderLibrary& GetInstance()
        {
            static ShaderLibrary instance;
            return instance;
        }
        void LoadStandartShaders();

    private:
        std::unordered_map<String, Ref<Shader>> m_Shaders;
    };
}
