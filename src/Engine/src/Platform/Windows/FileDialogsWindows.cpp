//
// Created by alexl on 05.06.2023.
//
// clang-format off
#include "Core/Application.h"
#include "JobSystem/JobScheduler.h"
#if defined(WINDOWS)
#include "Utils/FileDialogs.h"
#include "WindowsString.h"

#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#if defined(BEE_COMPILE_SDL)
#include "SDL3/SDL.h"
#endif
#include "Windowing/WindowHandler/WindowHandler.h"
// clang-format on
constexpr int MAX_DIR_SIZE = 256;
namespace BeeEngine
{
    void OpenFileWindowImpl(std::wstring& wFilter, Path& result)
    {
        OPENFILENAMEW ofn;
        WCHAR szFile[MAX_PATH] = {0};
        WCHAR currentDir[MAX_DIR_SIZE] = {0};

        ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        auto nativeInfo = WindowHandler::GetInstance()->GetNativeInfo();
        ofn.hwndOwner = (HWND)nativeInfo.window;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryW(MAX_DIR_SIZE, currentDir))
            ofn.lpstrInitialDir = currentDir;
        else
        {
            BeeCoreError("Failed to get current directory");
            return;
        }
        ofn.lpstrFilter = wFilter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameW(&ofn) == TRUE)
        {
            result = Internal::WStringToUTF8(ofn.lpstrFile);
        }
    }
    Path FileDialogs::OpenFile(Filter filter)
    {
        Path result;
        auto strFilter = GetFilter(&filter);

        auto wFilter = Internal::WStringFromUTF8(strFilter);

        if (Application::IsMainThread())
        {
            OpenFileWindowImpl(wFilter, result);
            return result;
        }
        BeeExpects(Jobs::this_job::IsInJob());
        Jobs::Counter counter;
        counter.Increment();
        Application::SubmitToMainThread(
            [&wFilter, &result, &counter]()
            {
                OpenFileWindowImpl(wFilter, result);
                counter.Decrement();
            });
        Job::WaitForJobsToComplete(counter);
        return result;
    }
    void SaveFileImpl(std::wstring& wFilter, Path& result)
    {
        OPENFILENAMEW ofn;
        WCHAR szFile[MAX_PATH] = {0};
        WCHAR currentDir[MAX_DIR_SIZE] = {0};

        ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        auto nativeInfo = WindowHandler::GetInstance()->GetNativeInfo();
        ofn.hwndOwner = (HWND)nativeInfo.window;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        if (GetCurrentDirectoryW(MAX_DIR_SIZE, currentDir))
            ofn.lpstrInitialDir = currentDir;
        else
        {
            BeeCoreError("Failed to get current directory");
            return;
        }
        ofn.lpstrFilter = wFilter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetSaveFileNameW(&ofn) == TRUE)
        {
            result = Internal::WStringToUTF8(ofn.lpstrFile);
        }
    }
    Path FileDialogs::SaveFile(Filter filter)
    {
        auto strFilter = GetFilter(&filter);

        auto wFilter = Internal::WStringFromUTF8(strFilter);

        Path result;
        if (Application::IsMainThread())
        {
            SaveFileImpl(wFilter, result);
            return result;
        }
        BeeExpects(Jobs::this_job::IsInJob());
        Jobs::Counter counter;
        counter.Increment();
        Application::SubmitToMainThread(
            [&wFilter, &result, &counter]()
            {
                SaveFileImpl(wFilter, result);
                counter.Decrement();
            });
        Job::WaitForJobsToComplete(counter);
        return result;
    }

    std::string FileDialogs::GetFilter(void* filter)
    {
        return ((FileDialogs::Filter*)filter)->WindowsFilter();
    }

    void OpenFolderImpl(Path& result)
    {
        WCHAR path[MAX_PATH];

        BROWSEINFOW bi = {0};
        ZeroMemory(&bi, sizeof(BROWSEINFOW));
        // bi.lpszTitle  = L"Выберите папку:";
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        bi.lpfn = nullptr;
        bi.lParam = 0;

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

            result = Internal::WStringToUTF8(path);
        }
    }

    Path FileDialogs::OpenFolder()
    {
        Path result;
        if (Application::IsMainThread())
        {
            OpenFolderImpl(result);
            return result;
        }
        BeeExpects(Jobs::this_job::IsInJob());
        Jobs::Counter counter;
        counter.Increment();
        Application::SubmitToMainThread(
            [&result, &counter]()
            {
                OpenFolderImpl(result);
                counter.Decrement();
            });
        Job::WaitForJobsToComplete(counter);
        return result;
    }
} // namespace BeeEngine
#endif