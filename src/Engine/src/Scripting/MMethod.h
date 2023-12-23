//
// Created by alexl on 30.07.2023.
//

#pragma once

#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class MClass;
    class MMethod
    {
        friend class MObject;
    public:
        MMethod(MClass& mClass, const String& name, int paramCount);
        ~MMethod();

        bool IsValid() const { return /*m_MonoMethod != nullptr;*/ false; }

        //operator MonoMethod*() const { return m_MonoMethod; }
    private:
        //MonoMethod* m_MonoMethod = nullptr;
        MClass* m_Class = nullptr;
        String m_Name;
        int m_ParamCount;
    };
}
