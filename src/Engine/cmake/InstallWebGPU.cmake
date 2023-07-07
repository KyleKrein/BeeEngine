include_guard()
set(BEE_WEBGPU_PATCH_TAG bd9125486881d7952d183f5d240117a696d1c151)

if (APPLE)
    set(USE_VULKAN OFF)
    set(USE_METAL ON)
else()
    set(USE_VULKAN ON)
    set(USE_METAL OFF)
endif()
set(DAWN_ENABLE_D3D11 OFF)
#if(WIN32)
#	set(DAWN_ENABLE_D3D12 ON)
#else()
set(DAWN_ENABLE_D3D12 OFF)
#endif ()
set(DAWN_ENABLE_METAL ${USE_METAL})
set(DAWN_ENABLE_NULL OFF)
set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)
set(DAWN_ENABLE_VULKAN ${USE_VULKAN})
set(TINT_BUILD_SPV_READER ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DAWN_ALWAYS_ASSERT ON)
endif ()
add_compile_definitions($<$<CONFIG:Debug>:DAWN_DEBUG_BREAK_ON_ERROR=1>)

# Disable unneeded parts
set(DAWN_BUILD_SAMPLES OFF)
set(TINT_BUILD_TINT ON)
set(TINT_BUILD_SAMPLES OFF)
set(TINT_BUILD_DOCS OFF)
set(TINT_BUILD_TESTS OFF)
set(TINT_BUILD_FUZZERS OFF)
set(TINT_BUILD_SPIRV_TOOLS_FUZZER OFF)
set(TINT_BUILD_AST_FUZZER OFF)
set(TINT_BUILD_REGEX_FUZZER OFF)
set(TINT_BUILD_BENCHMARKS OFF)
set(TINT_BUILD_TESTS OFF)
set(TINT_BUILD_AS_OTHER_OS OFF)
set(TINT_BUILD_REMOTE_COMPILE OFF)
set(DAWN_FETCH_DEPENDENCIES ON CACHE BOOL "Fetch dependencies from the internet")
#FetchContent_MakeAvailable(webgpu)
if(WIN32)
    message("If CPM says \"Cache for webgpu_dawn is dirty\", don't worry. It happens because of patches for Windows")
endif ()
CPMAddPackage(
        NAME webgpu_dawn
        GITHUB_REPOSITORY hexops/dawn
        GIT_TAG ${BEE_WEBGPU_PATCH_TAG}
        FIND_PACKAGE_ARGUMENTS
)

if (WIN32) #Apply patches to exclude d3d and WINUI
    if(NOT BEE_WEBGPU_PATCH_APPLIED OR NOT BEE_WEBGPU_PATCH_TAG_CACHE STREQUAL BEE_WEBGPU_PATCH_TAG)
        message("Applying patches for WebGPU for Windows")
        file(COPY
                ${CMAKE_CURRENT_LIST_DIR}/PatchesForWebGPU/printer_windows.cc
                DESTINATION ${webgpu_dawn_SOURCE_DIR}/src/tint/diagnostic/
                )
        file(COPY
                ${CMAKE_CURRENT_LIST_DIR}/PatchesForWebGPU/Surface.h
                DESTINATION ${webgpu_dawn_SOURCE_DIR}/src/dawn/native/
                )
        file(COPY
                ${CMAKE_CURRENT_LIST_DIR}/PatchesForWebGPU/Surface.cpp
                DESTINATION ${webgpu_dawn_SOURCE_DIR}/src/dawn/native/
                )
        set(BEE_WEBGPU_PATCH_APPLIED ON CACHE INTERNAL "WebGPU patches applied" FORCE)
        set(BEE_WEBGPU_PATCH_TAG_CACHE ${BEE_WEBGPU_PATCH_TAG} CACHE INTERNAL "WebGPU patch tag" FORCE)
    endif ()
endif ()


function(enable_webgpu target)
    message("Enabling WebGPU for ${target}")
    target_include_directories( ${target} PRIVATE ${webgpu_dawn_SOURCE_DIR}/include)
    target_include_directories( ${target} PRIVATE ${CMAKE_BINARY_DIR}/_deps/webgpu_dawn-build/gen/include)
    target_link_libraries( ${target} PRIVATE webgpu_dawn)
endfunction()