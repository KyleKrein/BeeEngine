#[[
CPMAddPackage(
        NAME freetype
        GITHUB_REPOSITORY freetype/freetype
        GIT_TAG 4d8db130ea4342317581bab65fc96365ce806b77
        FIND_PACKAGE_ARGUMENTS
)

CPMAddPackage(
        NAME msdfgen
        GITHUB_REPOSITORY Chlumsky/msdfgen
        GIT_TAG 755c45b4445baa12bfead90fa7d4d2259f2a14c4
)

get_property(MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Version is specified in vcpkg.json
project(msdfgen LANGUAGES CXX)

if(MAX_WARNING_LEVEL)
    if (MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

file(GLOB_RECURSE MSDFGEN_CORE_HEADERS ${msdfgen_SOURCE_DIR}/core/*.hpp)
file(GLOB_RECURSE MSDFGEN_CORE_HEADERS2 ${msdfgen_SOURCE_DIR}/core/*.h)
file(GLOB_RECURSE MSDFGEN_CORE_SOURCES ${msdfgen_SOURCE_DIR}/core/*.cpp)
file(GLOB_RECURSE MSDFGEN_EXT_HEADERS ${msdfgen_SOURCE_DIR}/ext/*.h)
file(GLOB_RECURSE MSDFGEN_EXT_HEADERS2 ${msdfgen_SOURCE_DIR}/ext/*.hpp)
file(GLOB_RECURSE MSDFGEN_EXT_SOURCES ${msdfgen_SOURCE_DIR}/ext/*.cpp)
file(GLOB_RECURSE MSDFGEN_EXT_SOURCES2 ${msdfgen_SOURCE_DIR}/lib/*.cpp)



# Core library
add_library(msdfgen-core "${msdfgen_SOURCE_DIR}/msdfgen.h" ${MSDFGEN_CORE_HEADERS} ${MSDFGEN_CORE_HEADERS2} ${MSDFGEN_CORE_SOURCES})
add_library(msdfgen::msdfgen-core ALIAS msdfgen-core)
set_target_properties(msdfgen-core PROPERTIES PUBLIC_HEADER "${MSDFGEN_CORE_HEADERS}")
target_compile_definitions(msdfgen-core PUBLIC
        MSDFGEN_VERSION=${MSDFGEN_VERSION}
        MSDFGEN_VERSION_MAJOR=${MSDFGEN_VERSION_MAJOR}
        MSDFGEN_VERSION_MINOR=${MSDFGEN_VERSION_MINOR}
        MSDFGEN_VERSION_REVISION=${MSDFGEN_VERSION_REVISION}
        MSDFGEN_COPYRIGHT_YEAR=${MSDFGEN_COPYRIGHT_YEAR}
)
target_include_directories(msdfgen-core INTERFACE
        $<INSTALL_INTERFACE:include/msdfgen>
        $<BUILD_INTERFACE:${msdfgen_SOURCE_DIR}/>
)
#set_property(DIRECTORY ${msdfgen_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT msdfgen-core)

if(MSDFGEN_USE_CPP11)
    target_compile_features(msdfgen-core PUBLIC cxx_std_11)
    target_compile_definitions(msdfgen-core PUBLIC MSDFGEN_USE_CPP11)
endif()

target_compile_definitions(msdfgen-core PUBLIC MSDFGEN_PUBLIC=)

add_library(msdfgen-ext "${msdfgen_SOURCE_DIR}/msdfgen-ext.h" ${MSDFGEN_EXT_HEADERS} ${MSDFGEN_EXT_SOURCES} ${MSDFGEN_EXT_HEADERS2} ${MSDFGEN_EXT_SOURCES2})
add_library(msdfgen::msdfgen-ext ALIAS msdfgen-ext)
set_target_properties(msdfgen-ext PROPERTIES PUBLIC_HEADER "${MSDFGEN_EXT_HEADERS} ${MSDFGEN_EXT_HEADERS2}")
target_compile_definitions(msdfgen-ext INTERFACE MSDFGEN_EXTENSIONS)
target_compile_definitions(msdfgen-ext PUBLIC MSDFGEN_DISABLE_SVG)
#target_compile_definitions(msdfgen-ext INTERFACE MSDFGEN_DISABLE_PNG)
find_package(freetype REQUIRED)
target_link_libraries(msdfgen-ext PRIVATE freetype msdfgen::msdfgen-core)
target_include_directories(msdfgen-ext
        PUBLIC
        $<INSTALL_INTERFACE:include/msdfgen>
        $<BUILD_INTERFACE:${msdfgen_SOURCE_DIR}>
        PRIVATE
        ${msdfgen_SOURCE_DIR}/include
        ${freetype_SOURCE_DIR}/include
        ${freetype_BINARY_DIR}/include
        ${freetype_SOURCE_DIR}/include/freetype
)
target_compile_definitions(msdfgen-ext PUBLIC MSDFGEN_EXT_PUBLIC=)

add_library(msdfgen-full INTERFACE)
add_library(msdfgen::msdfgen ALIAS msdfgen-full)
target_link_libraries(msdfgen-full INTERFACE msdfgen::msdfgen-core msdfgen::msdfgen-ext)

# Hide ZERO_CHECK and ALL_BUILD targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER meta)
]]
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(MSDF_ATLAS_DYNAMIC_RUNTIME TRUE)
else()
        set(MSDF_ATLAS_DYNAMIC_RUNTIME FALSE)
endif()
set(MSDFGEN_DISABLE_PNG ON)
set(MSDF_ATLAS_USE_SKIA OFF)

CPMAddPackage(
        NAME msdfatlasgen
        GITHUB_REPOSITORY Chlumsky/msdf-atlas-gen
        GIT_TAG 7e8d34645a67c6e4def5d281f7adec1c2501dc49
)
#[[
project(msdf-atlas-gen VERSION 1.2 LANGUAGES CXX)

find_package(Threads REQUIRED)

file(GLOB_RECURSE msdfatlasgen_HEADERS ${msdfatlasgen_SOURCE_DIR}/msdf-atlas-gen/*.h)
file(GLOB_RECURSE msdfatlasgen_HEADERS2 ${msdfatlasgen_SOURCE_DIR}/msdf-atlas-gen/*.hpp)

file(GLOB_RECURSE msdfatlasgen_SOURCES ${msdfatlasgen_SOURCE_DIR}/msdf-atlas-gen/*.cpp)

# msdf-atlas-gen library
add_library(msdf-atlas-gen ${msdfatlasgen_SOURCES} ${msdfatlasgen_HEADERS} ${msdfatlasgen_HEADERS2})
add_library(msdf-atlas-gen::msdf-atlas-gen ALIAS msdf-atlas-gen)
set_target_properties(msdf-atlas-gen PROPERTIES PUBLIC_HEADER "${msdfatlasgen_HEADERS}")
target_include_directories(msdf-atlas-gen
        INTERFACE
        $<BUILD_INTERFACE:${msdfatlasgen_SOURCE_DIR}>
        PRIVATE
        ${msdfatlasgen_SOURCE_DIR}/msdfgen/include # for lodepng.h
        ${msdfatlasgen_SOURCE_DIR}/artery-font-format
)

if (MSVC)
    target_compile_definitions(msdf-atlas-gen PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()
target_compile_features(msdf-atlas-gen PUBLIC cxx_std_11)
target_link_libraries(msdf-atlas-gen PUBLIC Threads::Threads msdfgen::msdfgen msdfgen-ext)
]]
add_library(MSDFGEN INTERFACE)
target_link_libraries(MSDFGEN INTERFACE msdf-atlas-gen::msdf-atlas-gen)
