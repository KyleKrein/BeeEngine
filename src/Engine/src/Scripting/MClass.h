//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "MField.h"

namespace BeeEngine
{
    class MAssembly;
    class ScriptingEngine;
    class MObject;
    class MMethod;
    class MField;

    class MClass
    {
        friend class MAssembly;
        friend class ScriptingEngine;
        friend class MObject;
        friend class MMethod;
        friend class MField;

    public:
        [[nodiscard]] const String& GetName() const { return m_Name; }
        [[nodiscard]] const String& GetNamespace() const { return m_Namespace; }
        MObject Instantiate();
        MMethod& GetMethod(const String& name, ManagedBindingFlags flags);
        MField& GetField(const String& name);

        const std::unordered_map<String, class MMethod>& GetMethods() const { return m_Methods; }
        std::unordered_map<String, class MField>& GetFields() { return m_Fields; }

        [[nodiscard]] const String& GetFullName() const { return m_FullName; }

        bool IsDerivedFrom(const MClass& other) const;
        bool IsEnum() const { return m_IsEnum; }
        bool IsValueType() const { return m_IsValueType; }
        MClass(const String& name, const String& ns, uint64_t contextId, uint64_t assemblyId, uint64_t classId);

        uint64_t GetContextID() const { return m_ContextID; }
        uint64_t GetAssemblyID() const { return m_AssemblyID; }
        uint64_t GetClassID() const { return m_ClassID; }

    private:
        String m_Name;
        String m_Namespace;
        uint64_t m_ContextID;
        uint64_t m_AssemblyID;
        uint64_t m_ClassID;
        std::unordered_map<String, class MMethod> m_Methods;
        std::unordered_map<String, class MField> m_Fields;
        String m_FullName = m_Namespace + "." + m_Name;
        bool m_IsEnum;
        bool m_IsValueType;
    };
} // namespace BeeEngine
