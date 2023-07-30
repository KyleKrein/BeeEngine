//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class MClass
    {
        friend class MAssembly;
    public:
        const String& GetName() const { return m_Name; }
        const String& GetNamespace() const { return m_Namespace; }
    private:
        MClass(const String& name, const String& ns)
            : m_Name(name), m_Namespace(ns) {}
        String m_Name;
        String m_Namespace;
    };
}