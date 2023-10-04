//
// Created by Александр Лебедев on 04.10.2023.
//

#pragma once

#include "FileSystem/FileWatcher.h"
#if defined(MACOS)
#include <CoreServices/CoreServices.h>
#endif

namespace BeeEngine::Internal
{
    class MacOSFileWatcher: public FileWatcher
    {
        friend void MacOSFileWatcherThread(MacOSFileWatcher& watcher);

#if defined(MACOS)
        friend void FileSystemEventCallback(
                ConstFSEventStreamRef streamRef,
                void *clientCallBackInfo,
                size_t numEvents,
                void *eventPaths,
                const FSEventStreamEventFlags eventFlags[],
                const FSEventStreamEventId eventIds[]
        );
#endif
    public:
        MacOSFileWatcher(const Path& path, const std::function<void(Path, Event)> &callback);
        void Start() override;
        void Stop() override;
        bool IsRunning() const override
        {
            return m_Running.load();
        }
        ~MacOSFileWatcher() override
        {
            if(MacOSFileWatcher::IsRunning())
                MacOSFileWatcher::Stop();
        }
    private:
        std::atomic<bool> m_Running = false;
        const Path m_Path;
        const std::function<void(Path, Event)> m_Callback;
#if defined(__cpp_lib_jthread)
        Scope<std::jthread> m_Thread = nullptr;
#else
        Scope<std::thread> m_Thread = nullptr;
#endif
#if defined(MACOS)
        FSEventStreamRef m_Stream;
        CFRunLoopRef m_RunLoop = nullptr;
#endif
    };
}