//
// Created by alexl on 30.09.2023.
//

#pragma once
#include <FileSystem/FileWatcher.h>
#include <thread>

namespace BeeEngine::Internal
{
    class WindowsFileWatcher : public FileWatcher
    {
        friend void WindowsFileWatcherThread(WindowsFileWatcher& watcher);

    public:
        WindowsFileWatcher(const Path& path);
        void Start() override;
        void Stop() override;
        bool IsRunning() const override { return m_Running.load(); }
        ~WindowsFileWatcher() override
        {
            if (WindowsFileWatcher::IsRunning())
                WindowsFileWatcher::Stop();
        }

    private:
        std::atomic<bool> m_Running = false;
        const std::wstring m_Path;
        Path m_OldNamePath;
#if defined(WINDOWS)
        friend void ProcessDirectoryChanges(WindowsFileWatcher& self, wchar_t* filename, unsigned char* Buffer);
        Scope<std::jthread> m_Thread = nullptr;
#endif
    };
} // namespace BeeEngine::Internal
