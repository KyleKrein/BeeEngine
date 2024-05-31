//
// Created by alexl on 30.09.2023.
//

#include "FileWatcher.h"
#include "Core/Application.h"
#include "File.h"
#include "Platform/MacOS/MacOSFileWatcher.h"
#include "Platform/STDFileWatcher.h"
#include "Platform/Windows/WindowsFileWatcher.h"

namespace BeeEngine
{

    Scope<FileWatcher> FileWatcher::Create(const Path& path, const std::function<void(Path, Event)>& callback)
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
            case OSPlatform::Mac:
                result = CreateScope<Internal::MacOSFileWatcher>(path, callback);
                break;
            case OSPlatform::Android:
            case OSPlatform::Linux:
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
} // namespace BeeEngine
