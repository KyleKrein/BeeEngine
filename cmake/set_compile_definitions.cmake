function(DesktopPlatformsOnly)
    add_compile_definitions(DESKTOP_PLATFORM)
endfunction(DesktopPlatformsOnly)

function(MobilePlatformsOnly)
    add_compile_definitions(MOBILE_PLATFORM)
endfunction(MobilePlatformsOnly)

if(IOS)
    # Covers iOS implementation
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)
elseif(APPLE)
    # Covers macOS implementation
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)
elseif(ANDROID)
    # Covers Android implementation
    MobilePlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)
elseif(WIN32)
    # Covers Windows implementation
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL OFF)
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)
endif()

function(DesktopPlatformsOnlyPostTarget)


endfunction(DesktopPlatformsOnlyPostTarget)

if(${BEE_COMPILE_WEBGPU})
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/InstallWebGPU.cmake)
endif ()

if(IOS)
    # Covers iOS implementation
    add_compile_definitions(IOS)
elseif(APPLE)
    # Covers macOS implementation
    add_compile_definitions(MACOS)
    DesktopPlatformsOnlyPostTarget()
elseif(ANDROID)
    # Covers Android implementation
    add_compile_definitions(ANDROID)
elseif(WIN32)
    # Covers Windows implementation
    add_compile_definitions(WINDOWS)
    DesktopPlatformsOnlyPostTarget()
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    add_compile_definitions(LINUX)
    DesktopPlatformsOnlyPostTarget()
endif()

if(${BEE_COMPILE_WEBGPU})
  message("Compiling WebGPU")
  add_compile_definitions(BEE_COMPILE_WEBGPU)
endif()

if (${BEE_COMPILE_SDL})
  message("Compiling with SDL3")
  add_compile_definitions(BEE_COMPILE_SDL)
endif()

if(${BEE_COMPILE_VULKAN})
  message("Compiling with Vulkan")
  add_compile_definitions(BEE_COMPILE_VULKAN)
endif()

if (${BEE_USE_VCPKG})
function(extract_vcpkg_path ICU_LIB_LIST RESULT_VAR)
    list(GET ICU_LIB_LIST 0 ICU_LIB_PATH)
    string(REGEX REPLACE "(.*/vcpkg_installed/[^/]+).*$" "\\1" EXTRACTED_PATH ${ICU_LIB_PATH})
    set(${RESULT_VAR} ${EXTRACTED_PATH} PARENT_SCOPE)
endfunction()

extract_vcpkg_path("${ICU_LIBRARIES}" ICU_VCPKG_PATH)
set(ICU_INCLUDE_DIR ${ICU_VCPKG_PATH}/include)
endif()

add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(GLM_FORCE_LEFT_HANDED)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_compile_definitions(BEE_ENABLE_CHECKS)

add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_ASSERTS>)
add_compile_definitions($<$<CONFIG:Release>:BEE_ENABLE_ASSERTS>)

add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_PROFILING>)
add_compile_definitions($<$<CONFIG:Release>:BEE_ENABLE_PROFILING>) # should be disabled in the future by default?

add_compile_definitions($<$<CONFIG:Debug>:BEE_VULKAN_ENABLE_VALIDATION_LAYERS>)

add_compile_definitions($<$<CONFIG:Release>:RELEASE>)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "Publish")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_compile_options(-O0)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Publish")
    add_compile_options(-O3)
endif()

if(NOT ${BEE_NO_DOTNET})
  add_compile_definitions(BEE_ENABLE_SCRIPTING)
endif()
