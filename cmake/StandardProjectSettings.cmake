# Set a default build type if none was specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(
            STATUS
            "Setting build type to 'Debug' as none was specified.")
    set(
            CMAKE_BUILD_TYPE Debug
            CACHE STRING "Choose the type of build." FORCE)

    # Set possible build type values for cmake-gui and ccmake
    set_property(
            CACHE CMAKE_BUILD_TYPE
            PROPERTY STRINGS "Debug" "Release")
endif ()

# Use ccache for faster rebuilds
find_program(CCACHE ccache)
if (CCACHE)
    message(STATUS "Using ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else ()
    message(STATUS "Ccache not found")
endif ()

# Generate compile_commands.json to make it easier to work
# with clang based tools. Used in combination with Ninja.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Debug option that also enables asserts and profiling
option(DEBUG "Enable debug statements" OFF)
if (DEBUG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(RELEASE)
endif ()