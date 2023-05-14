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
        auto& name = shader->GetName();
        Add(name, shader);
    }

    void ShaderLibrary::Add(Ref<String> name, const Ref<Shader>& shader)
    {
        if (Exists(*name))
        {
            BeeCoreWarn("Shader already exists");
            return;
        }
        m_Shaders[*name] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const String& filepath)
    {
        auto shader = Shader::Create(GetNameFromFilePath(filepath),filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(Ref<String> name, const String& filepath)
    {
        auto shader = Shader::Create(name,filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const String& name)
    {
        if (!Exists(name))
        {
            BeeCoreWarn("Shader not found");
            return nullptr;
        }
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const String& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }

    Ref<String> ShaderLibrary::GetNameFromFilePath(const String &filepath)
    {
        String result = ResourceManager::ProcessFilePath(filepath);
        int lastDot = result.find_last_of('.');
        int lastSlash = result.find_last_of('/') + 1;
        int count = lastDot == -1? result.size() - lastSlash: lastDot - lastSlash;
        return CreateRef<String>(result.substr(lastSlash, count));
    }
}
