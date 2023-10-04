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
        struct FileInfo
        {
            std::filesystem::file_time_type LastWriteTime;
            std::uintmax_t FileSize = 0;
            bool operator==(const FileInfo& other) const
            {
                return LastWriteTime == other.LastWriteTime && FileSize == other.FileSize;
            }
        };
        //USE FILE SIZE
        // Time interval at which we check the base folder for changes
        constexpr std::chrono::duration<int, std::milli> delay = std::chrono::milliseconds(500);// Why 500ms? Because it's a good compromise between responsiveness and performance
        std::unordered_map<Path, FileInfo> paths;
        for(auto& file : std::filesystem::recursive_directory_iterator(watcher.m_Path))
        {
            auto& path = file.path();
            paths[path] = {std::filesystem::last_write_time(file), std::filesystem::file_size(file)};

        }
        std::this_thread::sleep_for(delay);
        while (watcher.IsRunning())
        {
            std::unordered_map<Path, FileInfo> removed;
            std::unordered_map<Path, FileInfo> added;
            auto it = paths.begin();
            // Check if a file was deleted
            while (it != paths.end())
            {
                if(!File::Exists(it->first))
                {
                    //watcher.m_Callback(it->first, FileWatcher::Event::Removed);
                    removed[it->first] = it->second;
                    it = paths.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            for(auto& file : std::filesystem::recursive_directory_iterator(watcher.m_Path))
            {
                Path path = file.path();
                auto currentLastWriteTime = std::filesystem::last_write_time(file);
                auto currentFileSize = std::filesystem::file_size(file);
                if(!paths.contains(path))
                {
                    paths[path] = {currentLastWriteTime, currentFileSize};
                    //watcher.m_Callback(path, FileWatcher::Event::Added);
                    added[path] = {currentLastWriteTime, currentFileSize};
                }
                else if(paths.at(path).LastWriteTime != currentLastWriteTime)
                {
                    paths[path] =  {currentLastWriteTime, currentFileSize};
                    watcher.m_Callback(path, FileWatcher::Event::Modified);
                }
            }
            std::vector<Path> renamed;
            for(auto& [path, info] : removed)
            {
                static Path p;
                bool wasRenamed = false;
                for(auto& [addedPath, addedInfo] : added)
                {
                    if(addedInfo == info &&
                    addedPath.GetParent() == path.GetParent() &&
                    addedPath.GetFileName() != path.GetFileName())
                    {
                        watcher.m_Callback(path, FileWatcher::Event::RenamedOldName);
                        watcher.m_Callback(addedPath, FileWatcher::Event::RenamedNewName);
                        wasRenamed = true;
                        renamed.push_back(addedPath);
                        break;
                    }
                }
                if(wasRenamed)
                {
                    continue;
                }
                watcher.m_Callback(path, FileWatcher::Event::Removed);
            }
            for(auto& [path, info] : added)
            {
                if(std::find(renamed.begin(), renamed.end(), path) != renamed.end())
                    continue;
                watcher.m_Callback(path, FileWatcher::Event::Added);
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

    STDFileWatcher::STDFileWatcher(const Path &path, const std::function<void(Path, Event)> &callback)
    : m_Path(path.ToStdPath()), m_Callback(callback)
    {

    }
}
