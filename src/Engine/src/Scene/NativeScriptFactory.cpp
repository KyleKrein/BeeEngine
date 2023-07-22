//
// Created by alexl on 22.07.2023.
//

#include "NativeScriptFactory.h"


namespace BeeEngine
{
    NativeScriptFactory* NativeScriptFactory::s_Instance = nullptr;
    void NativeScriptFactory::RegisterScript(const char *name, NativeScriptFieldInfo *fields,
                                             unsigned long long int fieldsCount)
    {
        auto stringName = std::string(name);
        BeeCoreTrace("Registering script {0}", stringName);
        m_Hash.insert(stringName);
        m_NativeScripts.emplace_back(NativeScriptInfo{stringName,
                                                      fieldsCount ?
                                                      std::vector<NativeScriptFieldInfo>(fields, fields + fieldsCount) :
                                                              std::vector<NativeScriptFieldInfo>()});
    }

    ScriptableEntity* NativeScriptFactory::Create(const std::string &name)
    {
        BeeExpects(m_Data.CreateNativeScript_ClientOwned != nullptr);
        if(!IsScriptRegistered(name))
        {
            BeeCoreError("Script with name {} not found", name);
            return nullptr;
        }
        return reinterpret_cast<ScriptableEntity*>(m_Data.CreateNativeScript_ClientOwned(name.c_str()));
    }

    const std::vector<NativeScriptInfo> &NativeScriptFactory::GetNativeScripts()
    {
        return m_NativeScripts;
    }

    void NativeScriptFactory::Reload()
    {
        m_NativeScripts.clear();
        m_Hash.clear();
        m_Data.CreateNativeScript_ClientOwned = nullptr;
    }

    bool NativeScriptFactory::IsScriptRegistered(const std::string &name)
    {
        return m_Hash.contains(name);
    }

    NativeScriptFactory::NativeScriptFactory(BeeEngineNativeScriptRegistryData &data)
    : m_Data(data)
    {
        BeeExpects(s_Instance == nullptr);
        s_Instance = this;
        m_Data.RegisterScript_HostOwned = [](const char *name, NativeScriptFieldInfo *fields,
                                                 unsigned long long int fieldsCount){
            s_Instance->RegisterScript(name, fields, fieldsCount);
        };
    }
}
