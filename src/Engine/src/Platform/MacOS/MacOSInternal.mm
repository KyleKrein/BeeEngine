#include "MacOSInternal.h"
#include <Cocoa/Cocoa.h>
namespace BeeEngine::Internal::MacOS
{
    Path GetResourcesPathForCurrentBundle()
    {
        NSBundle *mainBundle = [NSBundle mainBundle];
        NSString *resourcePath = [mainBundle resourcePath];
        return Path(String([resourcePath UTF8String]));
    }
}