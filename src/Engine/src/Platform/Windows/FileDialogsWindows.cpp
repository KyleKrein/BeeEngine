//
// Created by alexl on 05.06.2023.
//
#if defined(WINDOWS)
#include "Utils/FileDialogs.h"

#include <windows.h>
#include <commdlg.h>
#if defined(BEE_COMPILE_GLFW)
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#else
#include "SDL3/SDL.h"
#include "SDL3/SDL_syswm.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#endif
namespace BeeEngine
{
    std::string FileDialogs::OpenFile(Filter filter)
    {
        auto strFilter = GetFilter(&filter);
        const char* f = strFilter.c_str();
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
#if defined(BEE_COMPILE_GLFW)
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
#else
        SDL_SysWMinfo wmInfo;
        SDL_GetWindowWMInfo((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &wmInfo, SDL_SYSWM_CURRENT_VERSION);
        ofn.hwndOwner = wmInfo.info.win.window;
#endif
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
        return {};
    }
    std::string FileDialogs::SaveFile(Filter filter)
    {
        auto strFilter = GetFilter(&filter);
        const char* f = strFilter.c_str();
        OPENFILENAMEA ofn;      // common dialog box structure
        CHAR szFile[260] = { 0 };      // if using TCHAR macros
        CHAR currentDir[256] = { 0 };
        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
#if defined(BEE_COMPILE_GLFW)
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
#else
        SDL_SysWMinfo wmInfo;
        SDL_GetWindowWMInfo((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &wmInfo, SDL_SYSWM_CURRENT_VERSION);

        ofn.hwndOwner = wmInfo.info.win.window;
#endif
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
        return {};
    }

    std::string FileDialogs::GetFilter(void* filter)
    {
        return ((FileDialogs::Filter*)filter)->WindowsFilter();
    }
}
#endif