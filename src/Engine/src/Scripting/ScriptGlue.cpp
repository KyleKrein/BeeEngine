//
// Created by alexl on 30.07.2023.
//

#include "ScriptGlue.h"
#include "ScriptingEngine.h"


namespace BeeEngine
{

    void ScriptGlue::RegisterInternalCalls()
    {
        BEE_INTERNAL_CALL(Log_Warn);
        BEE_INTERNAL_CALL(Log_Info);
        BEE_INTERNAL_CALL(Log_Error);
        BEE_INTERNAL_CALL(Log_Trace);
    }

    void ScriptGlue::Log_Warn(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeWarn(msg);
        mono_free(msg);
    }

    void ScriptGlue::Log_Info(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeInfo(msg);
        mono_free(msg);
    }

    void ScriptGlue::Log_Error(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeError(msg);
        mono_free(msg);
    }

    void ScriptGlue::Log_Trace(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeTrace(msg);
        mono_free(msg);
    }
}
