//
// Created by alexl on 21.07.2023.
//

#pragma once
namespace BeeEngine
{
    struct NativeScriptFieldInfo
    {
        const char* Type;
        const char* Name;
    };

    struct INativeScriptRegistry
    {
        virtual ~INativeScriptRegistry() = default;
        virtual void
        RegisterScript(const char* name, NativeScriptFieldInfo* fields, unsigned long long fieldsCount) = 0;
    };
} // namespace BeeEngine

struct BeeEngineNativeScriptRegistryData
{
    void (*RegisterScript_HostOwned)(const char* name,
                                     BeeEngine::NativeScriptFieldInfo* fields,
                                     unsigned long long fieldsCount) = nullptr;
    void* (*CreateNativeScript_ClientOwned)(const char* name) = nullptr;
};