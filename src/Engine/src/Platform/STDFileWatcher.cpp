//
// Created by alexl on 30.09.2023.
//

#include "STDFileWatcher.h"
#include "Core/Application.h"
#include "FileSystem/File.h"
#include "JobSystem/JobScheduler.h"
#include <chrono>
#include <filesystem>

namespace BeeEngine::Internal
{

    void STDFileWatcherThread(STDFileWatcher& watcher)
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
        // USE FILE SIZE
        //  Time interval at which we check the base folder for changes
        constexpr std::chrono::duration<int, std::milli> delay = std::chrono::milliseconds(
            500); // Why 500ms? Because it's a good compromise between responsiveness and performance
        std::unordered_map<Path, FileInfo> paths;
        bool mainIsDirectory = std::filesystem::is_directory(watcher.m_Path);
        if (mainIsDirectory)
        {
            for (auto& file : std::filesystem::recursive_directory_iterator(watcher.m_Path))
            {
                auto& path = file.path();
                paths[path] = {std::filesystem::last_write_time(file),
                               std::filesystem::is_directory(file) ? 0 : std::filesystem::file_size(file)};
            }
        }
        else
        {
            paths[watcher.m_Path] = {std::filesystem::last_write_time(watcher.m_Path),
                                     std::filesystem::file_size(watcher.m_Path)};
        }
        Jobs::this_job::SleepFor(delay);
        while (watcher.IsRunning())
        {
            std::unordered_map<Path, FileInfo> removed;
            std::unordered_map<Path, FileInfo> added;
            auto it = paths.begin();
            // Check if a file was deleted
            while (it != paths.end())
            {
                if (!File::Exists(it->first))
                {
                    // watcher.m_Callback(it->first, FileWatcher::Event::Removed);
                    removed[it->first] = it->second;
                    it = paths.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            if (!paths.empty() && !mainIsDirectory)
            {
                auto currentLastWriteTime = std::filesystem::last_write_time(watcher.m_Path);
                auto currentFileSize = std::filesystem::file_size(watcher.m_Path);
                if (paths.at(watcher.m_Path).LastWriteTime != currentLastWriteTime)
                {
                    paths[watcher.m_Path] = {currentLastWriteTime, currentFileSize};
                    watcher.onAnyEvent.emit(watcher.m_Path, FileWatcher::Event::Modified);
                    watcher.onFileModified.emit(watcher.m_Path);
                }
            }
            else
            {
                for (auto& file : std::filesystem::recursive_directory_iterator(watcher.m_Path))
                {
                    Path path = file.path();
                    auto currentLastWriteTime = std::filesystem::last_write_time(file);
                    auto currentFileSize = std::filesystem::is_directory(file) ? 0 : std::filesystem::file_size(file);
                    if (!paths.contains(path))
                    {
                        paths[path] = {currentLastWriteTime, currentFileSize};
                        // watcher.m_Callback(path, FileWatcher::Event::Added);
                        added[path] = {currentLastWriteTime, currentFileSize};
                    }
                    else if (paths.at(path).LastWriteTime != currentLastWriteTime)
                    {
                        paths[path] = {currentLastWriteTime, currentFileSize};
                        watcher.onAnyEvent.emit(path, FileWatcher::Event::Modified);
                        watcher.onFileModified.emit(path);
                    }
                }
            }
            std::vector<Path> renamed;
            for (auto& [path, info] : removed)
            {
                static Path p;
                bool wasRenamed = false;
                for (auto& [addedPath, addedInfo] : added)
                {
                    if (addedInfo == info && addedPath.GetParent() == path.GetParent() &&
                        addedPath.GetFileName() != path.GetFileName())
                    {
                        watcher.onAnyEvent.emit(path, FileWatcher::Event::RenamedOldName);
                        watcher.onAnyEvent.emit(addedPath, FileWatcher::Event::RenamedNewName);
                        watcher.onFileRenamed.emit(path, addedPath);
                        wasRenamed = true;
                        renamed.push_back(addedPath);
                        break;
                    }
                }
                if (wasRenamed)
                {
                    continue;
                }
                watcher.onAnyEvent.emit(path, FileWatcher::Event::Removed);
                watcher.onFileRemoved.emit(path);
            }
            for (auto& [path, info] : added)
            {
                if (std::find(renamed.begin(), renamed.end(), path) != renamed.end())
                    continue;
                watcher.onAnyEvent.emit(path, FileWatcher::Event::Added);
                watcher.onFileAdded.emit(path);
            }
            if (!mainIsDirectory && paths.empty())
            {
                Application::SubmitToMainThread(
                    [&watcher, p = watcher.m_Path]()
                    {
                        BeeCoreTrace("File watcher {0} stopped because file was deleted", p);
                        watcher.Stop();
                    });
                break;
            }

            Jobs::this_job::SleepFor(delay);
        }
    }

    void STDFileWatcher::Start()
    {
        if (IsRunning())
        {
            return;
        }
        m_Running.store(true);
        Jobs::Schedule(
            Jobs::CreateJob<Jobs::Priority::Low, Jobs::DefaultStackSize>(m_JobCounter, STDFileWatcherThread, *this));
    }

    void STDFileWatcher::Stop()
    {
        m_Running.store(false);
        Jobs::WaitForJobsToComplete(m_JobCounter);
    }

    bool STDFileWatcher::IsRunning() const
    {
        return m_Running.load();
    }

    STDFileWatcher::STDFileWatcher(const Path& path) : m_Path(path.ToStdPath()) {}
} // namespace BeeEngine::Internal
