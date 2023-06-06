//
// Created by alexl on 05.06.2023.
//
#if defined(WINDOWS)
#include "Utils/FileDialogs.h"

#include <windows.h>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
namespace BeeEngine
{
    std::optional<std::string> FileDialogs::OpenFile(Filter filter)
    {
        const char* f = GetFilter(&filter);
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = f;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return std::nullopt;
    }
    std::optional<std::string> FileDialogs::SaveFile(Filter filter)
    {
        const char* f = GetFilter(&filter);
        OPENFILENAMEA ofn;      // common dialog box structure
        CHAR szFile[260] = { 0 };      // if using TCHAR macros
        CHAR currentDir[256] = { 0 };
        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = f;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return std::nullopt;
    }

    const char *FileDialogs::GetFilter(void* filter)
    {
        return ((FileDialogs::Filter*)filter)->WindowsFilter().c_str();
    }
}
#endif