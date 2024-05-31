//
// Created by alexl on 30.07.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include "MTypes.h"

namespace BeeEngine
{
    class MClass;
    class MMethod
    {
        friend class MObject;

    public:
        MMethod(MClass& mClass, const String& name, ManagedBindingFlags flags);
        ~MMethod();

        bool IsValid() const { return m_MethodID; }
        void* InvokeStatic(void** params);

        MClass& GetClass();

        // operator MonoMethod*() const { return m_MonoMethod; }
    private:
        // MonoMethod* m_MonoMethod = nullptr;
        uint64_t m_MethodID = 0;
        MClass* m_Class = nullptr;
        String m_Name;
    };
} // namespace BeeEngine
