//
// Created by alexl on 30.09.2023.
//

#pragma once
#include <FileSystem/FileWatcher.h>
#include <filesystem>
#include <unordered_map>
#include <version>
#include <thread>
#include <chrono>

namespace BeeEngine::Internal
{
    class STDFileWatcher: public FileWatcher
    {
        friend void STDFileWatcherThread(STDFileWatcher& watcher);

    public:
        STDFileWatcher(const Path& path, const std::function<void(Path, Event)> &callback);
        void Start() override;

        void Stop() override;

        bool IsRunning() const override;

        ~STDFileWatcher() override
        {
            if(STDFileWatcher::IsRunning())
                STDFileWatcher::Stop();
        }

    public:

    private:
        std::atomic<bool> m_Running = false;
        const std::filesystem::path m_Path;
        const std::function<void(Path, Event)> m_Callback;
#if defined(__cpp_lib_jthread)
        Scope<std::jthread> m_Thread = nullptr;
#else
        Scope<std::thread> m_Thread = nullptr;
#endif
    };
}
