//
// Created by alexl on 05.06.2023.
//

#if defined(MACOS)
#include "Utils/FileDialogs.h"
#include "Cocoa/Cocoa.h"

std::optional<std::string> BeeEngine::FileDialogs::OpenFile(const char* filter)
{
    filter = GetFilter(filter);
    // Create the File Open Dialog class.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];

    //[openDlg setAllowedFileTypes:[NSArray arrayWithObject:@filter]];

// Enable the selection of files in the dialog.
    [openDlg setCanChooseFiles:YES];

// Multiple files not allowed
    [openDlg setAllowsMultipleSelection:NO];

// Can't select a directory
    [openDlg setCanChooseDirectories:NO];
    if ([openDlg runModal] == NSModalResponseOK) {
            NSURL *nsurl = [[openDlg URLs] objectAtIndex:0];
            // nsurl.path contains the NSString I want to return as std::string
            return std::string([[[nsurl absoluteURL] path] UTF8String]);
    }
    return std::nullopt;
}

std::optional<std::string> BeeEngine::FileDialogs::SaveFile(const char* filter)
{
    filter = GetFilter(filter);
    NSSavePanel *saveDlg = [NSSavePanel savePanel];
    //[saveDlg setAllowedFileTypes:[NSArray arrayWithObject:[NSString stringWithUTF8String:filter]]];
    if ([saveDlg runModal] == NSModalResponseOK) {
        NSString *nsurl = [[saveDlg URL] path];
        // nsurl.path contains the NSString I want to return as std::string
        return std::string([nsurl UTF8String]);
    }
    return std::nullopt;
}

const char *BeeEngine::FileDialogs::GetFilter(const char *filter)
{
    std::string firstString = {filter};
    std::string secondString = {firstString.c_str() + firstString.size() + 1 + 1};
    return filter;
}
#endif
