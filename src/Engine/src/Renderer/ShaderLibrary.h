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
        void Add(Ref<String> name, const Ref<Shader>& shader);
        Ref<Shader> Load(const String& filepath);
        Ref<Shader> Load(Ref<String> name, const String& filepath);

        Ref<Shader> Get(const String& name);
        bool Exists(const String& name) const;

    private:
        std::unordered_map<String, Ref<Shader>> m_Shaders;

        Ref<String> GetNameFromFilePath(const String &filepath);
    };
}
