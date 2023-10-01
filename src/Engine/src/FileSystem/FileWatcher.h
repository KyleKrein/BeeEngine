//
// Created by alexl on 30.09.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include "Core/Path.h"

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

        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual bool IsRunning() const = 0;

        virtual ~FileWatcher() = default;

        static Scope<FileWatcher> Create(const Path& path, const std::function<void(const Path&, Event)>& callback);
    };
}
