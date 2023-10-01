//
// Created by alexl on 30.09.2023.
//

#include "STDFileWatcher.h"
#include <chrono>
#include <filesystem>
#include "FileSystem/File.h"


namespace BeeEngine::Internal
{

    void STDFileWatcherThread(STDFileWatcher &watcher)
    {
        // Time interval at which we check the base folder for changes
        constexpr std::chrono::duration<int, std::milli> delay = std::chrono::milliseconds(500);// Why 500ms? Because it's a good compromise between responsiveness and performance
        std::unordered_map<Path, std::filesystem::file_time_type> paths;
        for(auto& file : std::filesystem::recursive_directory_iterator(watcher.m_Path))
        {
            paths[file.path()] = std::filesystem::last_write_time(file);
        }
        std::this_thread::sleep_for(delay);
        while (watcher.IsRunning())
        {
            auto it = paths.begin();
            // Check if a file was deleted
            while (it != paths.end())
            {
                if(!File::Exists(it->first))
                {
                    watcher.m_Callback(it->first, FileWatcher::Event::Removed);
                    it = paths.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            for(auto& file : std::filesystem::recursive_directory_iterator(watcher.m_Path))
            {
                auto currentLastWriteTime = std::filesystem::last_write_time(file);
                if(!paths.contains(file.path()))
                {
                    paths[file.path()] = currentLastWriteTime;
                    watcher.m_Callback(file.path(), FileWatcher::Event::Added);
                }
                else if(paths[file.path()] != currentLastWriteTime)
                {
                    paths[file.path()] = currentLastWriteTime;
                    watcher.m_Callback(file.path(), FileWatcher::Event::Modified);
                }
            }
            std::this_thread::sleep_for(delay);
        }
    }

    void STDFileWatcher::Start()
    {
        if(IsRunning())
            return;
        m_Running.store(true);
#if defined(__cpp_lib_jthread)
        m_Thread = CreateScope<std::jthread>(STDFileWatcherThread, std::ref(*this));
#else
        m_Thread = CreateScope<std::thread>(STDFileWatcherThread, std::ref(*this));
#endif
    }

    void STDFileWatcher::Stop()
    {
        m_Running.store(false);
#if !defined(__cpp_lib_jthread)
        if(m_Thread && m_Thread->joinable())
            m_Thread->join();
#endif
        m_Thread = nullptr;
    }

    bool STDFileWatcher::IsRunning() const
    {
        return m_Running.load();
    }

    STDFileWatcher::STDFileWatcher(const Path &path, const std::function<void(const Path &, Event)> &callback)
    : m_Path(path.ToStdPath()), m_Callback(callback)
    {

    }
}
