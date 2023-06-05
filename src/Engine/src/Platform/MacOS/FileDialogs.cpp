//
// Created by alexl on 05.06.2023.
//

#if defined(MACOS)
#include "Utils/FileDialogs.h"
#include "Cocoa/Cocoa.h"

std::optional<std::string> BeeEngine::FileDialogs::OpenFile(const char* filter)
{
    NSOpenPanel *op = [NSOpenPanel openPanel];
    if ([op runModal] == NSOKButton) {
            NSURL *nsurl = [[op URLs] objectAtIndex:0];
            // nsurl.path contains the NSString I want to return as std::string
            return std::string([[nsurl path] UTF8String]);
    }
    return std::nullopt;
}

std::optional<std::string> BeeEngine::FileDialogs::SaveFile(const char* filter)
{
    return std::nullopt;
}
#endif
