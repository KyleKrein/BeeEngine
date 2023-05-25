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

    void ShaderLibrary::Add(std::string_view name, const Ref<Shader>& shader)
    {
        BEE_PROFILE_FUNCTION();
        if (Exists(name))
        {
            BeeCoreWarn("Shader already exists");
            return;
        }
        m_Shaders[String(name)] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(std::string_view filepath)
    {
        BEE_PROFILE_FUNCTION();
        auto shader = Shader::Create(ResourceManager::GetNameFromFilePath(filepath),filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(std::string_view name, std::string_view filepath)
    {
        BEE_PROFILE_FUNCTION();
        auto shader = Shader::Create(name,filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(std::string_view name) const
    {
        BEE_PROFILE_FUNCTION();
        return m_Shaders.at(String(name));
    }

    bool ShaderLibrary::Exists(std::string_view name) const
    {
        BEE_PROFILE_FUNCTION();
        return m_Shaders.find(String(name)) != m_Shaders.end();
    }
}
