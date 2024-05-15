//
// Created by alexl on 05.06.2023.
//

#if defined(MACOS)
#include "Utils/FileDialogs.h"
#include "JobSystem/JobScheduler.h"
#include "Core/Application.h"
#include "Cocoa/Cocoa.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
BeeEngine::Path BeeEngine::FileDialogs::OpenFile(FileDialogs::Filter filter)
{
    if(Application::IsMainThread())
    {
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        auto strFilter = GetFilter(&filter);
        NSString *f = [NSString stringWithUTF8String:strFilter.c_str()];
        //[openDlg setAllowedFileTypes:[NSArray arrayWithObject:@filter]];
        [openDlg setAllowedFileTypes:[NSArray arrayWithObject:f]];
        // Enable the selection of files in the dialog.
        [openDlg setCanChooseFiles:YES];

        // Multiple files not allowed
        [openDlg setAllowsMultipleSelection:NO];
        // Can't select a directory
        [openDlg setCanChooseDirectories:NO];

        if ([openDlg runModal] == NSModalResponseOK) {
            NSURL *nsurl = [[openDlg URLs] objectAtIndex:0];
            // nsurl.path contains the NSString I want to return as std::string
            NSString *path = [[nsurl absoluteURL] path];
            return std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        }
        return {};
    }
    BeeExpects(Jobs::this_job::IsInJob());
    // Create the File Open Dialog class.
    Jobs::Counter counter;
    counter.Increment();
    BeeEngine::Path result;
    Application::SubmitToMainThread([&counter, &result, filter]() mutable{
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        auto strFilter = GetFilter(&filter);
        NSString *f = [NSString stringWithUTF8String:strFilter.c_str()];
        //[openDlg setAllowedFileTypes:[NSArray arrayWithObject:@filter]];
        [openDlg setAllowedFileTypes:[NSArray arrayWithObject:f]];
        // Enable the selection of files in the dialog.
        [openDlg setCanChooseFiles:YES];

        // Multiple files not allowed
        [openDlg setAllowsMultipleSelection:NO];
        // Can't select a directory
        [openDlg setCanChooseDirectories:NO];

        if ([openDlg runModal] == NSModalResponseOK) {
            NSURL *nsurl = [[openDlg URLs] objectAtIndex:0];
            // nsurl.path contains the NSString I want to return as std::string
            NSString *path = [[nsurl absoluteURL] path];
            result = std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        }
        counter.Decrement();
    });
    Jobs::WaitForJobsToComplete(counter);
    return result;
}

BeeEngine::Path BeeEngine::FileDialogs::SaveFile(Filter filter)
{
    if(Application::IsMainThread())
    {
        auto strFilter = GetFilter(&filter);
        NSString *f = [NSString stringWithUTF8String:strFilter.c_str()];
        NSSavePanel *saveDlg = [NSSavePanel savePanel];
        [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:f]];
        if ([saveDlg runModal] == NSModalResponseOK) {
            NSString *path = [[saveDlg URL] path];
            // nsurl.path contains the NSString I want to return as std::string
            return std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        }
        return {};
    }
    BeeExpects(Jobs::this_job::IsInJob());
    BeeEngine::Path result;
    Jobs::Counter counter;
    counter.Increment();
    Application::SubmitToMainThread([&counter, &result, filter]() mutable{
        auto strFilter = GetFilter(&filter);
        NSString *f = [NSString stringWithUTF8String:strFilter.c_str()];
        NSSavePanel *saveDlg = [NSSavePanel savePanel];
        [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:f]];
        if ([saveDlg runModal] == NSModalResponseOK) {
            NSString *path = [[saveDlg URL] path];
            // nsurl.path contains the NSString I want to return as std::string
            result = std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        }
        counter.Decrement();
    });
    Jobs::WaitForJobsToComplete(counter);
    return result;
}

std::string BeeEngine::FileDialogs::GetFilter(void* filter)
{
    return std::string(((FileDialogs::Filter*)filter)->filter + 2);
}

BeeEngine::Path BeeEngine::FileDialogs::OpenFolder()
{
    if(Application::IsMainThread())
    {
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        [openDlg setCanChooseFiles:NO];
        [openDlg setCanChooseDirectories:YES];
        if ([openDlg runModal] == NSModalResponseOK) {
            NSURL *nsurl = [[openDlg URLs] objectAtIndex:0];
            // nsurl.path contains the NSString I want to return as std::string
            NSString *path = [[nsurl absoluteURL] path];
            return std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        }
        return {};
    }
    BeeExpects(Jobs::this_job::IsInJob());
    BeeEngine::Path result;
    Jobs::Counter counter;
    counter.Increment();
    Application::SubmitToMainThread([&counter, &result]() mutable{
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        [openDlg setCanChooseFiles:NO];
        [openDlg setCanChooseDirectories:YES];
        if ([openDlg runModal] == NSModalResponseOK) {
            NSURL *nsurl = [[openDlg URLs] objectAtIndex:0];
            // nsurl.path contains the NSString I want to return as std::string
            NSString *path = [[nsurl absoluteURL] path];
            result = std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        }
        counter.Decrement();
    });
    Jobs::WaitForJobsToComplete(counter);
    return result;
}
#pragma clang diagnostic pop
#endif
