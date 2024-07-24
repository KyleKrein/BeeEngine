//
// Created by alexl on 30.09.2023.
//

#pragma once

#include "Core/Path.h"
#include "Core/Property.h"
#include "Core/TypeDefines.h"
#include <utility>

namespace BeeEngine
{
    class FileWatcher
    {
    public:
        enum class Event
        {
            Added,
            Modified,
            Removed,
            RenamedOldName,
            RenamedNewName
        };

        Signal<const Path&, FileWatcher::Event> onAnyEvent;
        Signal<const Path&> onFileAdded;
        Signal<const Path&> onFileModified;
        Signal<const Path&> onFileRemoved;
        // First parameter is oldPath, second is newPath
        Signal<const Path&, const Path&> onFileRenamed;

        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual bool IsRunning() const = 0;

        virtual ~FileWatcher() = default;

        static Scope<FileWatcher> Create(const Path& path);
    };
} // namespace BeeEngine
