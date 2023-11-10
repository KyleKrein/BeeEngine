//
// Created by alexl on 09.11.2023.
//

#include "StackTrace.h"

namespace BeeEngine
{
#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
    StackTrace::StackTrace(const std::stacktrace& stacktrace)
    {
        m_Entries.reserve(stacktrace.size());
        for (const auto& entry : stacktrace)
        {
            m_Entries.emplace_back(std::move(entry.description()), std::move(entry.source_file()), entry.source_line());
        }
    }

#else

#endif
    String StackTrace::ToString() const
    {
        std::ostringstream stream;
        for(size_t i = 0; i < m_Entries.size(); ++i)
        {
            const auto& entry = m_Entries[i];
            stream << i << ": " << entry.Description << " at " << entry.FileName << ":" << entry.LineNumber << "\n";
        }
        return stream.str();
    }
} // BeeEngine