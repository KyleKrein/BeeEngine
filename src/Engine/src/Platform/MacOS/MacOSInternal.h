#include "Core/Path.h"
#include <exception>
namespace BeeEngine::Internal::MacOS
{
    Path GetResourcesPathForCurrentBundle();
#if !defined(MACOS)
    inline Path GetResourcesPathForCurrentBundle()
    {
        throw std::runtime_error("GetResourcesPathForCurrentBundle is only available on MacOS");
    }
#endif
}