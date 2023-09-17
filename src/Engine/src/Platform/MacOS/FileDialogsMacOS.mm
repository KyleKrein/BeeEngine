//
// Created by alexl on 05.06.2023.
//

#if defined(MACOS)
#include "Utils/FileDialogs.h"
#include "Cocoa/Cocoa.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
Path BeeEngine::FileDialogs::OpenFile(FileDialogs::Filter filter)
{
    // Create the File Open Dialog class.
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
            std::string result = std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
            if(result.empty())
                return {};
            return result;
    }
    return {};
}

Path BeeEngine::FileDialogs::SaveFile(Filter filter)
{
    auto strFilter = GetFilter(&filter);
    NSString *f = [NSString stringWithUTF8String:strFilter.c_str()];
    NSSavePanel *saveDlg = [NSSavePanel savePanel];
    [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:f]];
    if ([saveDlg runModal] == NSModalResponseOK) {
        NSString *path = [[saveDlg URL] path];
        // nsurl.path contains the NSString I want to return as std::string
        std::string result = std::string([path UTF8String], [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
        if(result.empty())
            return {};
        return result;
    }
    return {};
}

std::string BeeEngine::FileDialogs::GetFilter(void* filter)
{
    return std::string(((FileDialogs::Filter*)filter)->filter + 2);
}
#pragma clang diagnostic pop
#endif
