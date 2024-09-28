//
// Created by alexl on 30.09.2023.
//

#pragma once
#include "JobSystem/JobScheduler.h"
#include <FileSystem/FileWatcher.h>
#include <chrono>
#include <filesystem>
#include <thread>
#include <unordered_map>
#include <version>

namespace BeeEngine::Internal
{
    class STDFileWatcher : public FileWatcher
    {
        friend void STDFileWatcherThread(STDFileWatcher& watcher);

    public:
        STDFileWatcher(const Path& path);
        void Start() override;

        void Stop() override;

        bool IsRunning() const override;

        ~STDFileWatcher() override
        {
            if (STDFileWatcher::IsRunning())
            {
                STDFileWatcher::Stop();
            }
        }

    private:
        std::atomic<bool> m_Running = false;
        const std::filesystem::path m_Path;
        Jobs::Counter m_JobCounter;
    };
} // namespace BeeEngine::Internal
