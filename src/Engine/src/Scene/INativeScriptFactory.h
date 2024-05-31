//
// Created by alexl on 22.07.2023.
//

#pragma once
#include <string>
#include <vector>
namespace BeeEngine
{
    class ScriptableEntity;
    struct INativeScriptFactory
    {
        virtual ~INativeScriptFactory() = default;
        virtual ScriptableEntity* Create(const std::string& name) = 0;
        virtual const std::vector<struct NativeScriptInfo>& GetNativeScripts() = 0;
        virtual void Reload() = 0;
        virtual bool IsScriptRegistered(const std::string& name) = 0;
    };
} // namespace BeeEngine