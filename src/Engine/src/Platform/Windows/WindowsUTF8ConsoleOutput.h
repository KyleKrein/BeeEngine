//
// Created by alexl on 09.10.2023.
//

#pragma once
#if defined(WINDOWS)
#include <windows.h>
#endif

namespace BeeEngine::Internal
{
    class WindowsUTF8ConsoleOutput final
    {
#if defined(WINDOWS)
    public:
        WindowsUTF8ConsoleOutput()
        : m_OriginalCodePage(GetConsoleOutputCP())
        {
            ::SetConsoleOutputCP(CP_UTF8);
            BeeCoreTrace("Set console output code page to UTF-8");
        }
        ~WindowsUTF8ConsoleOutput()
        {
            ::SetConsoleOutputCP(m_OriginalCodePage);
            BeeCoreTrace("Restored console output code page to {}", m_OriginalCodePage);
        }
    private:
        UINT m_OriginalCodePage;
#endif
    };
}
