//
// Created by alexl on 30.09.2023.
//

#include "FileWatcher.h"
#include "Platform/Windows/WindowsFileWatcher.h"
#include "Core/Application.h"
#include "Platform/STDFileWatcher.h"
#include "File.h"


namespace BeeEngine
{

    Scope<FileWatcher> FileWatcher::Create(const Path &path, const std::function<void(const Path &, Event)> &callback)
    {
        BeeExpects(!path.IsEmpty());
        BeeExpects(path.IsAbsolute());
        BeeExpects(File::Exists(path));
        Scope<FileWatcher> result = nullptr;
        switch (Application::GetOsPlatform())
        {
            case OSPlatform::Windows:
                result = CreateScope<Internal::WindowsFileWatcher>(path, callback);
                break;
            case OSPlatform::Linux:
            case OSPlatform::Mac:
            case OSPlatform::Android:
            case OSPlatform::iOS:
            default:
            {
                BeeCoreWarn("FileWatcher is not implemented for this platform. Using STDFileWatcher instead.");
                result = CreateScope<Internal::STDFileWatcher>(path, callback);
                break;
            }
        }
        result->Start();

        return result;
    }
}
