//
// Created by alexl on 05.06.2023.
//
#if defined(WINDOWS)
#include "Utils/FileDialogs.h"
#include "WindowsString.h"

#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#if defined(BEE_COMPILE_GLFW)
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#else
#include "SDL3/SDL.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#endif

constexpr int MAX_DIR_SIZE = 256;
namespace BeeEngine
{
    Path FileDialogs::OpenFile(Filter filter)
    {
        auto strFilter = GetFilter(&filter);

        auto wFilter = Internal::WStringFromUTF8(strFilter);

        OPENFILENAMEW ofn;
        WCHAR szFile[MAX_PATH] = { 0 };
        WCHAR currentDir[MAX_DIR_SIZE] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
        ofn.lStructSize = sizeof(OPENFILENAMEW);
#if defined(BEE_COMPILE_GLFW)
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
#else
        auto nativeInfo = WindowHandler::GetInstance()->GetNativeInfo();
        ofn.hwndOwner = (HWND)nativeInfo.window;
#endif
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryW(MAX_DIR_SIZE, currentDir))
            ofn.lpstrInitialDir = currentDir;
        else
        {
            BeeCoreError("Failed to get current directory");
            return {};
        }
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
        WCHAR szFile[MAX_PATH] = { 0 };
        WCHAR currentDir[MAX_DIR_SIZE] = { 0 };
        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
        ofn.lStructSize = sizeof(OPENFILENAMEW);
#if defined(BEE_COMPILE_GLFW)
        ofn.hwndOwner = glfwGetWin32Window(glfwGetCurrentContext());
#else
        auto nativeInfo = WindowHandler::GetInstance()->GetNativeInfo();
        ofn.hwndOwner = (HWND)nativeInfo.window;
#endif
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryW(MAX_DIR_SIZE, currentDir))
            ofn.lpstrInitialDir = currentDir;
        else
        {
            BeeCoreError("Failed to get current directory");
            return {};
        }
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
        WCHAR path[MAX_PATH];

        BROWSEINFOW bi = { 0 };
        ZeroMemory(&bi, sizeof(BROWSEINFOW));
        //bi.lpszTitle  = L"Выберите папку:";
        bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        bi.lpfn       = nullptr;
        bi.lParam     = 0;

        LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

        if (pidl != nullptr)
        {
            // Получаем путь из выбранного элемента и сохраняем его в переменной path
            SHGetPathFromIDListW(pidl, path);

            // Освобождаем память
            IMalloc* imalloc = nullptr;
            if (SUCCEEDED(SHGetMalloc(&imalloc)))
            {
                imalloc->Free(pidl);
                imalloc->Release();
            }

            return Internal::WStringToUTF8(path);
        }
        return {};
    }
}
#endif