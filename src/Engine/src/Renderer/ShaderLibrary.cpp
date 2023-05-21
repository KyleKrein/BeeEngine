//
// Created by alexl on 13.05.2023.
//

#include <regex>
#include "ShaderLibrary.h"
#include "Core/Logging/Log.h"
#include "Core/ResourceManager.h"


namespace BeeEngine
{
    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        BEE_PROFILE_FUNCTION();
        auto& name = shader->GetName();
        Add(name, shader);
    }

    void ShaderLibrary::Add(Ref<String> name, const Ref<Shader>& shader)
    {
        BEE_PROFILE_FUNCTION();
        if (Exists(*name))
        {
            BeeCoreWarn("Shader already exists");
            return;
        }
        m_Shaders[*name] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const String& filepath)
    {
        BEE_PROFILE_FUNCTION();
        auto shader = Shader::Create(ResourceManager::GetNameFromFilePath(filepath),filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(Ref<String> name, const String& filepath)
    {
        BEE_PROFILE_FUNCTION();
        auto shader = Shader::Create(name,filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const String& name)
    {
        BEE_PROFILE_FUNCTION();
        if (!Exists(name))
        {
            BeeCoreWarn("Shader not found");
            return nullptr;
        }
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const String& name) const
    {
        BEE_PROFILE_FUNCTION();
        return m_Shaders.find(name) != m_Shaders.end();
    }
}
