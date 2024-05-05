//
// Created by alexl on 30.09.2023.
//

#include "WindowsFileWatcher.h"

#include <atomic>
#include <future>
#include <thread>
#include <utility>
#if defined(WINDOWS)
#include "WindowsString.h"
#include <windows.h>
#endif

namespace BeeEngine::Internal
{
#if defined(WINDOWS)
    void ProcessDirectoryChanges(const std::function<void(const Path&, FileWatcher::Event)>& callback,
                                 wchar_t* filename,
                                 BYTE* Buffer);

    void ReadDirectoryChangesCustom(
        HANDLE hDir, BYTE* Buffer, size_t BufferSize, DWORD& BytesReturned, OVERLAPPED& Overlapped);
#endif

    void WindowsFileWatcherThread(WindowsFileWatcher& watcher)
    {
#if defined(WINDOWS)
        HANDLE hDir = CreateFileW(watcher.m_Path.c_str(),                                 // pointer to the file name
                                  FILE_LIST_DIRECTORY,                                    // access (read/write) mode
                                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
                                  NULL,                                                   // security descriptor
                                  OPEN_EXISTING,                                          // how to create
                                  FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,      // file attributes
                                  NULL // file with attributes to copy
        );
        wchar_t filename[MAX_PATH * 2];
        std::array<BYTE, 4096> Buffer;
        DWORD BytesReturned;
        OVERLAPPED Overlapped = {0};
        Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (nullptr == Overlapped.hEvent)
        {
            BeeCoreError("CreateEvent failed with error code: {}", GetLastError());
            return;
        }
        ReadDirectoryChangesCustom(hDir, Buffer.data(), Buffer.size(), BytesReturned, Overlapped);
        while (watcher.IsRunning())
        {
            DWORD dwWaitStatus = WaitForSingleObject(Overlapped.hEvent, 0);
            switch (dwWaitStatus)
            {
                case WAIT_TIMEOUT:

                    break;
                case WAIT_OBJECT_0:
                    GetOverlappedResult(hDir, &Overlapped, &BytesReturned, FALSE);
                    ProcessDirectoryChanges(watcher.m_Callback, filename, Buffer.data());
                    ResetEvent(Overlapped.hEvent);
                    ReadDirectoryChangesCustom(hDir, Buffer.data(), Buffer.size(), BytesReturned, Overlapped);
                    break;
                default:
                    break;
            }
        }
        CloseHandle(Overlapped.hEvent);
        CloseHandle(hDir);
#endif
    }
#if defined(WINDOWS)
    void ReadDirectoryChangesCustom(
        HANDLE hDir, BYTE* Buffer, size_t BufferSize, DWORD& BytesReturned, OVERLAPPED& Overlapped)
    {
        ReadDirectoryChangesW(hDir,       // handle to directory
                              Buffer,     // read results buffer
                              BufferSize, // length of buffer
                              TRUE,       // monitoring option
                              FILE_NOTIFY_CHANGE_SECURITY | FILE_NOTIFY_CHANGE_CREATION |
                                  FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_LAST_WRITE |
                                  FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                                  FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME, // filter conditions
                              &BytesReturned,                                                 // bytes returned
                              &Overlapped,                                                    // overlapped buffer
                              NULL                                                            // completion routine
        );
    }

    void ProcessDirectoryChanges(const std::function<void(const Path&, FileWatcher::Event)>& callback,
                                 wchar_t* filename,
                                 BYTE* Buffer)
    {
        BYTE* p = Buffer;
        while (true)
        {
            FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)p;
            wcscpy(filename, L"");

            wcsncpy(filename, pNotify->FileName, pNotify->FileNameLength / sizeof(wchar_t));
            filename[pNotify->FileNameLength / sizeof(wchar_t)] = NULL;
            Path path(WStringToUTF8(filename));

            switch (pNotify->Action)
            {
                case FILE_ACTION_MODIFIED:
                    callback(path, FileWatcher::Event::Modified);
                    break;
                case FILE_ACTION_ADDED:
                    callback(path, FileWatcher::Event::Added);
                    break;
                case FILE_ACTION_REMOVED:
                    callback(path, FileWatcher::Event::Removed);
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    callback(path, FileWatcher::Event::RenamedOldName);
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME:
                    callback(path, FileWatcher::Event::RenamedNewName);
                    break;
            }
            if (!pNotify->NextEntryOffset)
                break; // this was last entry
            p += pNotify->NextEntryOffset;
        }
    }
#endif
    WindowsFileWatcher::WindowsFileWatcher(const Path& path, const std::function<void(Path, Event)>& callback)
        : m_Path(std::move(path.ToStdPath().wstring())), m_Callback(callback)
    {
    }

    void WindowsFileWatcher::Start()
    {
#if defined(WINDOWS)
        if (m_Running)
            return;
        m_Running.store(true);
#if defined(__cpp_lib_jthread)
        m_Thread = CreateScope<std::jthread>(WindowsFileWatcherThread, std::ref(*this));
#else
        m_Thread = CreateScope<std::thread>(WindowsFileWatcherThread, std::ref(*this));
#endif
#endif
    }

    void WindowsFileWatcher::Stop()
    {
#if defined(WINDOWS)
        m_Running.store(false);
#if !defined(__cpp_lib_jthread)
        if (m_Thread && m_Thread->joinable())
            m_Thread->join();
#endif
        m_Thread = nullptr;
#endif
    }
} // namespace BeeEngine::Internal
