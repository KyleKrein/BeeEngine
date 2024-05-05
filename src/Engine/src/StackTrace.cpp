//
// Created by alexl on 09.11.2023.
//
#if !defined(__cpp_lib_stacktrace) || __cpp_lib_stacktrace < 202011L
#if defined(MACOS)
#include <dlfcn.h>
#include <execinfo.h>
#endif
#endif
#include "Core/Logging/Log.h"
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

#elif defined(MACOS)
    StackTrace::StackTrace()
    {
        constexpr static int32_t max_frames = 128;
        void* addrlist[max_frames + 1];
        int32_t addrLen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
        if (addrLen == 0)
        {
            BeeCoreWarn("No stack trace available");
            return;
        }
        m_Entries.reserve(addrLen);
        char** symbolList = backtrace_symbols(addrlist, addrLen);
        for (int i = 2; i < addrLen; i++)
        { // 2 to skip this function
            Dl_info info;
            if (dladdr(addrlist[i], &info) && info.dli_sname)
            {
                char* demangled = nullptr;
                int status = -1;
                if (info.dli_sname[0] == '_')
                {
                    demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
                }
                std::string caller = status == 0                 ? demangled
                                     : info.dli_sname == nullptr ? symbolList[i]
                                                                 : info.dli_sname;
                m_Entries.emplace_back(std::move(caller),
                                       info.dli_fname ? std::string(info.dli_fname) : std::string(),
                                       0 /* No line number */);
                free(demangled);
            }
            else
            {
                m_Entries.emplace_back(symbolList[i], "", 0);
            }
        }
        free(symbolList);
    }
#endif
    String StackTrace::ToString() const
    {
        std::ostringstream stream;
        for (size_t i = 0; i < m_Entries.size(); ++i)
        {
            const auto& entry = m_Entries[i];
            stream << i << ": " << entry.Description << " at " << entry.FileName << ":" << entry.LineNumber << "\n";
        }
        return stream.str();
    }
} // namespace BeeEngine