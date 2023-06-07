//
// Created by alexl on 13.05.2023.
//

#include <regex>
#include "ShaderLibrary.h"
#include "Core/Logging/Log.h"
#include "Core/ResourceManager.h"
#include "../../Assets/EmbeddedResources.h"


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

    void ShaderLibrary::LoadStandartShaders()
    {
        auto vertex2dshader = Internal::GetEmbeddedResource(EmbeddedResource::Standart2DShaderVertex);
        auto fragment2dshader = Internal::GetEmbeddedResource(EmbeddedResource::Standart2DShaderFragment);

        auto shader2d = Shader::Create("Standart2DShader", std::string_view((char*)vertex2dshader.data(), vertex2dshader.size()), std::string_view((char*)fragment2dshader.data(), fragment2dshader.size()));

        Add(shader2d);
    }
}
