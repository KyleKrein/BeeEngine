//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "mono/metadata/appdomain.h"

#define BEE_INTERNAL_CALL(name) ScriptingEngine::RegisterInternalCall("BeeEngine.Internal.InternalCalls::" #name, (void*)&name)
namespace BeeEngine
{
    class ScriptGlue
    {
    public:
        static void RegisterInternalCalls();
    private:
        static void Log_Warn(MonoString* message);
        static void Log_Info(MonoString* message);
        static void Log_Error(MonoString* message);
        static void Log_Trace(MonoString* message);

    };
}
