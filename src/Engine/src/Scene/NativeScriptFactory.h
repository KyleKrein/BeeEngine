//
// Created by alexl on 22.07.2023.
//

#pragma once

#include "INativeScriptRegistry.h"
#include "INativeScriptFactory.h"
#include <unordered_set>

namespace BeeEngine
{
    struct NativeScriptInfo
    {
        std::string Name;
        std::vector<NativeScriptFieldInfo> Fields;
    };

    class NativeScriptFactory: public INativeScriptFactory, public INativeScriptRegistry
    {
    public:

        static NativeScriptFactory& GetInstance()
        {
            return *s_Instance;
        }
        NativeScriptFactory(BeeEngineNativeScriptRegistryData& data);
        ~NativeScriptFactory() override = default;
        void RegisterScript(const char* name, NativeScriptFieldInfo* fields, unsigned long long fieldsCount) override;
        ScriptableEntity* Create(const std::string& name) override;
        const std::vector<NativeScriptInfo>& GetNativeScripts() override;
        void Reload() override;
        bool IsScriptRegistered(const std::string& name) override;
    private:
        std::vector<NativeScriptInfo> m_NativeScripts;
        std::unordered_set<std::string> m_Hash;
        BeeEngineNativeScriptRegistryData& m_Data;

        static NativeScriptFactory* s_Instance;
    };
}
