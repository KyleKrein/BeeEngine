//
// Created by alexl on 05.06.2023.
//
#if !defined(WINDOWS)
#include "Utils/FileDialogs.h"
#include "WindowsString.h"

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
    Path FileDialogs::OpenFile(Filter filter)
    {
        auto strFilter = GetFilter(&filter);

        auto wFilter = Internal::WStringFromUTF8(strFilter);

        OPENFILENAMEW ofn;
        WCHAR szFile[260] = { 0 };
        WCHAR currentDir[256] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
        ofn.lStructSize = sizeof(OPENFILENAMEW);
#if defined(BEE_COMPILE_GLFW)
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
#else
        SDL_SysWMinfo wmInfo;
        SDL_GetWindowWMInfo((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &wmInfo, SDL_SYSWM_CURRENT_VERSION);
        ofn.hwndOwner = wmInfo.info.win.window;
#endif
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryW(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = wFilter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameW(&ofn) == TRUE)
        {
            return Internal::WStringToUTF8(ofn.lpstrFile);
        }
        return {};
    }
    Path FileDialogs::SaveFile(Filter filter)
    {
        auto strFilter = GetFilter(&filter);

        auto wFilter = Internal::WStringFromUTF8(strFilter);

        OPENFILENAMEW ofn;      // common dialog box structure
        WCHAR szFile[260] = { 0 };      // if using TCHAR macros
        WCHAR currentDir[256] = { 0 };
        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
        ofn.lStructSize = sizeof(OPENFILENAMEW);
#if defined(BEE_COMPILE_GLFW)
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
#else
        SDL_SysWMinfo wmInfo;
        SDL_GetWindowWMInfo((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &wmInfo, SDL_SYSWM_CURRENT_VERSION);

        ofn.hwndOwner = wmInfo.info.win.window;
#endif
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryW(256, currentDir))
            ofn.lpstrInitialDir = currentDir;
        ofn.lpstrFilter = wFilter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetSaveFileNameW(&ofn) == TRUE)
        {
            return Internal::WStringToUTF8(ofn.lpstrFile);
        }
        return {};
    }

    std::string FileDialogs::GetFilter(void* filter)
    {
        return ((FileDialogs::Filter*)filter)->WindowsFilter();
    }

    Path FileDialogs::OpenFolder()
    {

    }

    Path FileDialogs::SaveFolder()
    {

    }
}
#endif