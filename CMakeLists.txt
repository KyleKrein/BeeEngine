cmake_minimum_required(VERSION 3.25)
include(cmake/UniversalAppleBuild.cmake)
option(BEE_USE_VCPKG ON)
option(BEE_USE_SYSTEM_SDL3 OFF)
option(BEE_BUILD_TESTS ON)
option(BEE_BUILD_SANDBOX OFF)
option(BEE_BUILD_LOC_TOOL OFF)
option(BEE_BUILD_EDITOR ON)
option(BEE_BUILD_RUNTIME ON)

message("Build Tests? ${BEE_BUILD_TESTS}")
message("Build Sandbox? ${BEE_BUILD_SANDBOX}")
message("Build Localization Tool? ${BEE_BUILD_LOC_TOOL}")
message("Build Editor? ${BEE_BUILD_EDITOR}")
message("Build Runtime? ${BEE_BUILD_RUNTIME}")

if(WIN32)
    execute_process(
        COMMAND wmic OS get OSArchitecture
        OUTPUT_VARIABLE HOST_ARCH
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(HOST_ARCH MATCHES "64-bit")
        set(BEE_ARCH "x64")
    else()
        set(BEE_ARCH ${HOST_ARCH})
    endif()
else()
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE HOST_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(HOST_ARCH STREQUAL "x86_64")
        set(BEE_ARCH "x64")
    elseif(HOST_ARCH STREQUAL "aarch64" OR HOST_ARCH STREQUAL "arm64")
        set(BEE_ARCH "arm64")
        set(ENV{VCPKG_FORCE_SYSTEM_BINARIES} arm)
    else()
        set(BEE_ARCH ${HOST_ARCH})
    endif()
endif()

message(STATUS "Detected processor architecture: ${BEE_ARCH}")


if(UNIX AND NOT APPLE)
        set(VCPKG_TARGET_TRIPLET ${BEE_ARCH}-linux)
	#add_compile_options(-march=nehalem) #doesnt build on arm
        #add_compile_options(-march=native)
        set(SDL_WAYLAND ON)
  if(BEE_ARCH STREQUAL "x64")
    add_compile_options(-march=x86-64-v3)
  elseif(BEE_ARCH STREQUAL "arm64")
    #add_compile_options(-march=)
  endif()
  add_compile_options(-Wno-changes-meaning) #fix for gcc giving error when i use ex. MeshSource MeshSource(); Function declaration
endif()

set(VCPKG_LIBRARY_LINKAGE static CACHE STRING "Linkage of vcpkg libraries" FORCE)
set(VCPKG_CRT_LINKAGE static CACHE STRING "Linkage of vcpkg libraries" FORCE)
#TODO: make this only for distribution builds, when dist builds are implemented
if(WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(VCPKG_TARGET_TRIPLET ${BEE_ARCH}-windows-static)
        if(POLICY CMP0091)
                cmake_policy(SET CMP0091 NEW) 
        endif()
        if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
        else()
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
        endif()
        add_compile_options(/O2)
        add_link_options(/LTCG:STATUS)
endif()
set(VCPKG_ROOT "src/Engine/vendor/vcpkg")
message("Using VCPKG? ${BEE_USE_VCPKG}")
if (${BEE_USE_VCPKG})
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
  include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()
project(
        BeeEngine_CPP
        DESCRIPTION "BeeEngine"
        LANGUAGES CXX C
)
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        include(CheckIPOSupported)
        # Optional IPO. Do not use IPO if it's not supported by compiler.
        check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX C)
        if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message("Using Link time optimizations")
        else()
                message(WARNING "Link time optimizations are not supported: ${output}")
        endif()
endif()
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BEEENGINE_DOTNET_VERSION "8.0")

include(cmake/StandardProjectSettings.cmake)

# Link project_warnings as "library" to use the warnings
# specified in CompilerWarnings.cmake.
#add_library(project_warnings INTERFACE)
#include(cmake/CompilerWarnings.cmake)
#set_project_warnings(project_warnings)

add_subdirectory(src/Engine)
if(${BEE_BUILD_SANDBOX})
  add_subdirectory(src/Sandbox2D)
endif()
if(${BEE_BUILD_TESTS})
  add_subdirectory(src/tests)
endif()
if(${BEE_BUILD_EDITOR})
  add_subdirectory(src/Editor)
endif()
if(${BEE_BUILD_LOC_TOOL})
  add_subdirectory(src/LocalizationTool)
endif()
if(${BEE_BUILD_RUNTIME})
  add_subdirectory(src/Runtime)
endif()

