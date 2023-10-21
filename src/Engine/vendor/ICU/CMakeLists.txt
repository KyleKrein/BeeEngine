cmake_minimum_required(VERSION 3.25)

set(BUILD_ICU ON CACHE BOOL "Build ICU" FORCE)
set(ICU_BUILD_VERSION "73.2" CACHE STRING "ICU version to build" FORCE)
set(ICU_STATIC ON CACHE BOOL "Build ICU static" FORCE)

CPMAddPackage(
        NAME ICU
        GITHUB_REPOSITORY viaduck/icu-cmake #https://github.com/viaduck/icu-cmake
        GIT_TAG 4b56d4ae0c7883c9cc7655c8357685ca34807754 #v73.2 set 21.10.23
)

function(link_icu_data target)
    if(WIN32)
        set(ICU_LIB "${CMAKE_BINARY_DIR}/_deps/icu-build/icu_host-build/stubdata/libicudata.lib")
    elseif(APPLE OR UNIX)
        set(ICU_LIB "${CMAKE_BINARY_DIR}/_deps/icu-build/icu_host-build/stubdata/libicudata.a")
    endif()

    find_library(ICU_DATA NAMES ${ICU_LIB} REQUIRED)
    target_link_libraries(${target} INTERFACE ${ICU_DATA})
endfunction()

add_library(ICU INTERFACE)
target_link_libraries(ICU INTERFACE icu)
link_icu_data(ICU)
