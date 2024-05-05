//
// Created by Александр Лебедев on 04.10.2023.
//

#include "MacOSFileWatcher.h"

namespace BeeEngine::Internal
{
#if defined(MACOS)
    void FileSystemEventCallback(ConstFSEventStreamRef streamRef,
                                 void* clientCallBackInfo,
                                 size_t numEvents,
                                 void* eventPaths,
                                 const FSEventStreamEventFlags eventFlags[],
                                 const FSEventStreamEventId eventIds[])
    {
        char** paths = (char**)eventPaths;
        bool oldRenamed = true;
        for (int i = 0; i < numEvents; i++)
        {
            Path path = paths[i];
            MacOSFileWatcher* watcher = (MacOSFileWatcher*)clientCallBackInfo;
            if (eventFlags[i] & kFSEventStreamEventFlagItemCreated)
                watcher->m_Callback(path, FileWatcher::Event::Added);
            else if (eventFlags[i] & kFSEventStreamEventFlagItemRemoved)
                watcher->m_Callback(path, FileWatcher::Event::Removed);
            else if (eventFlags[i] & kFSEventStreamEventFlagItemRenamed)
            {
                if (oldRenamed)
                    watcher->m_Callback(path, FileWatcher::Event::RenamedOldName);
                else
                    watcher->m_Callback(path, FileWatcher::Event::RenamedNewName);
                oldRenamed = !oldRenamed;
            }
            else if (eventFlags[i] & kFSEventStreamEventFlagItemModified)
                watcher->m_Callback(path, FileWatcher::Event::Modified);
        }
    }
#endif
    void MacOSFileWatcherThread(MacOSFileWatcher& watcher)
    {
#if defined(MACOS)
        CFStringRef mypath = CFStringCreateWithCString(NULL, watcher.m_Path.AsCString(), kCFStringEncodingUTF8);
        CFArrayRef pathsToWatch = CFArrayCreate(NULL, (const void**)&mypath, 1, NULL);
        void* callbackInfo = &watcher;
        FSEventStreamContext context = {0, callbackInfo, nullptr, nullptr, nullptr};
        watcher.m_Stream = FSEventStreamCreate(NULL,
                                               &FileSystemEventCallback,
                                               &context,
                                               pathsToWatch,
                                               kFSEventStreamEventIdSinceNow,
                                               0.1,
                                               kFSEventStreamCreateFlagFileEvents);
        watcher.m_RunLoop = CFRunLoopGetCurrent();
        FSEventStreamScheduleWithRunLoop(watcher.m_Stream, watcher.m_RunLoop, kCFRunLoopDefaultMode);
        FSEventStreamStart(watcher.m_Stream);
        CFRunLoopRun();
        CFRelease(pathsToWatch);
        CFRelease(mypath);
#endif
    }

    MacOSFileWatcher::MacOSFileWatcher(const Path& path, const std::function<void(Path, Event)>& callback)
        : m_Path(path), m_Callback(callback)
    {
    }

    void MacOSFileWatcher::Start()
    {
        if (IsRunning())
            return;
        m_Running.store(true);
#if defined(__cpp_lib_jthread)
        m_Thread = CreateScope<std::jthread>(MacOSFileWatcherThread, std::ref(*this));
#else
        m_Thread = CreateScope<std::thread>(MacOSFileWatcherThread, std::ref(*this));
#endif
    }

    void MacOSFileWatcher::Stop()
    {
        m_Running.store(false);
#if defined(MACOS)
        FSEventStreamStop(m_Stream);
        FSEventStreamInvalidate(m_Stream);
        FSEventStreamRelease(m_Stream);
        if (m_RunLoop)
        {
            CFRunLoopStop(m_RunLoop); // Останавливаем RunLoop
        }
#endif
#if !defined(__cpp_lib_jthread)
        if (m_Thread && m_Thread->joinable())
            m_Thread->join();
#endif
        m_Thread = nullptr;
    }
} // namespace BeeEngine::Internal