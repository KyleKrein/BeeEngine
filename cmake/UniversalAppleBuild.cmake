# Generate universal executable for Apple hardware.
# This file needs to be included before calling `project`.
#if (APPLE AND NOT IOS AND RELEASE)
if (APPLE AND NOT IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)
endif ()
#elseif (APPLE AND NOT IOS AND DEBUG)
#    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "")
#endif ()