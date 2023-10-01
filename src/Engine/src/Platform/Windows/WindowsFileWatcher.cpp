//
// Created by alexl on 30.09.2023.
//

#include "WindowsFileWatcher.h"

#include <utility>
#include <thread>
#include <future>
#include <atomic>
#if defined(WINDOWS)
#include <windows.h>
#include "WindowsString.h"
#endif


namespace BeeEngine::Internal
{
#if defined(WINDOWS)
    void ProcessDirectoryChanges(const std::function<void(const Path&, FileWatcher::Event)> &callback, wchar_t *filename, const FILE_NOTIFY_INFORMATION *Buffer);

    void
    ReadDirectoryChangesCustom(HANDLE hDir, FILE_NOTIFY_INFORMATION *Buffer, DWORD &BytesReturned,
                               OVERLAPPED &Overlapped);
#endif

    void WindowsFileWatcherThread(WindowsFileWatcher& watcher)
    {
#if defined(WINDOWS)
        HANDLE hDir = CreateFileW(watcher.m_Path.c_str(), // pointer to the file name
                                 FILE_LIST_DIRECTORY,                // access (read/write) mode
                                 FILE_SHARE_READ | FILE_SHARE_WRITE |FILE_SHARE_DELETE,  // share mode
                                 NULL,                               // security descriptor
                                 OPEN_EXISTING,                      // how to create
                                 FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,         // file attributes
                                 NULL                                // file with attributes to copy
        );
        wchar_t filename[MAX_PATH];
        FILE_NOTIFY_INFORMATION Buffer[1024];
        DWORD BytesReturned;
        OVERLAPPED Overlapped = {0};
        Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if(nullptr == Overlapped.hEvent)
        {
            BeeCoreError("CreateEvent failed with error code: {}", GetLastError());
            return;
        }
        ReadDirectoryChangesCustom(hDir, Buffer, BytesReturned, Overlapped);
        while (watcher.IsRunning())
        {
            DWORD dwWaitStatus = WaitForSingleObject(Overlapped.hEvent, 0);
            switch(dwWaitStatus)
            {
                case WAIT_TIMEOUT:

                    break;
                case WAIT_OBJECT_0:
                    GetOverlappedResult(hDir, &Overlapped, &BytesReturned, FALSE);
                    ProcessDirectoryChanges(watcher.m_Callback, filename, Buffer);
                    ResetEvent(Overlapped.hEvent);
                    ReadDirectoryChangesCustom(hDir, Buffer, BytesReturned, Overlapped);
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
    void
    ReadDirectoryChangesCustom(HANDLE hDir, FILE_NOTIFY_INFORMATION *Buffer, DWORD &BytesReturned,
                               OVERLAPPED &Overlapped)
    {
        ReadDirectoryChangesW(
                hDir,                                  // handle to directory
                Buffer,                                    // read results buffer
                1024,                                // length of buffer
                TRUE,                                 // monitoring option
                FILE_NOTIFY_CHANGE_SECURITY |
                FILE_NOTIFY_CHANGE_CREATION |
                FILE_NOTIFY_CHANGE_LAST_ACCESS |
                FILE_NOTIFY_CHANGE_LAST_WRITE |
                FILE_NOTIFY_CHANGE_SIZE |
                FILE_NOTIFY_CHANGE_ATTRIBUTES |
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_FILE_NAME,            // filter conditions
                &BytesReturned,              // bytes returned
                &Overlapped,                          // overlapped buffer
                NULL// completion routine
        );
    }


    void ProcessDirectoryChanges(const std::function<void(const Path&, FileWatcher::Event)> &callback, wchar_t *filename, const FILE_NOTIFY_INFORMATION *Buffer)
    {
        int offset = 0;
        FILE_NOTIFY_INFORMATION* pNotify;
        pNotify = (FILE_NOTIFY_INFORMATION*)((char*)Buffer + offset);
        wcscpy(filename, L"");

        wcsncpy(filename, pNotify->FileName, pNotify->FileNameLength / 2);

        filename[pNotify->FileNameLength / 2] = NULL;
        Path path(WStringToUTF8(filename));

        switch (Buffer[0].Action)
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
    }
#endif
    WindowsFileWatcher::WindowsFileWatcher(const Path& path, const std::function<void(const Path &, Event)> &callback)
    : m_Path(std::move(path.ToStdPath().wstring())), m_Callback(callback)
    {

    }

    void WindowsFileWatcher::Start()
    {
#if defined(WINDOWS)
        if(m_Running)
            return;
        m_Running.store(true);
        m_Thread = CreateScope<std::jthread>(WindowsFileWatcherThread, std::ref(*this));
#endif
    }

    void WindowsFileWatcher::Stop()
    {
#if defined(WINDOWS)
        m_Running.store(false);
        m_Thread = nullptr;
#endif
    }
}
