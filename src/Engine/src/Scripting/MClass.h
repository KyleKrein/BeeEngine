//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"

extern "C"
{
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoImage MonoImage;
}

namespace BeeEngine
{
    class MClass
    {
        friend class MAssembly;
        friend class ScriptingEngine;
        friend class MObject;
        friend class MMethod;
    public:
        [[nodiscard]] const String& GetName() const { return m_Name; }
        [[nodiscard]] const String& GetNamespace() const { return m_Namespace; }
        MObject Instantiate();
        MMethod& GetMethod(const String& name, int paramCount);
        [[nodiscard]] const String& GetFullName() const { return m_FullName; }

        bool IsDerivedFrom(MClass& other) const;
    private:
        MClass(const String& name, const String& ns, MonoImage* image);
        String m_Name;
        String m_Namespace;
        MonoClass* m_MonoClass = nullptr;
        std::unordered_map<String, class MMethod> m_Methods;
        String m_FullName = m_Namespace + "." + m_Name;
    };
}