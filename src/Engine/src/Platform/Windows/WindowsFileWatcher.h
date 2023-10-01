//
// Created by alexl on 30.09.2023.
//

#pragma once
#include <FileSystem/FileWatcher.h>
#include <thread>

namespace BeeEngine::Internal
{
    class WindowsFileWatcher: public FileWatcher
    {
        friend void WindowsFileWatcherThread(WindowsFileWatcher& watcher);
    public:
        WindowsFileWatcher(const Path& path, const std::function<void(const Path &, Event)> &callback);
        void Start() override;
        void Stop() override;
        bool IsRunning() const override
        {
            return m_Running.load();
        }
        ~WindowsFileWatcher() override
        {
            if(WindowsFileWatcher::IsRunning())
                WindowsFileWatcher::Stop();
        }
    private:
        std::atomic<bool> m_Running = false;
        const std::wstring m_Path;
        const std::function<void(const Path &, Event)> m_Callback;
#if defined(WINDOWS)
        Scope<std::jthread> m_Thread = nullptr;
#endif
    };
}
