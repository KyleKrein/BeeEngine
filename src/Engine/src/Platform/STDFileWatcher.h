//
// Created by alexl on 30.09.2023.
//

#pragma once
#include <FileSystem/FileWatcher.h>
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <chrono>

namespace BeeEngine::Internal
{
    class STDFileWatcher: public FileWatcher
    {
        friend void STDFileWatcherThread(STDFileWatcher& watcher);

    public:
        STDFileWatcher(const Path& path, const std::function<void(const Path &, Event)> &callback);
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
        const std::function<void(const Path &, Event)> m_Callback;
        Scope<std::jthread> m_Thread = nullptr;
    };
}
