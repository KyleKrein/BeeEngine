cmake_minimum_required(VERSION 3.14)

CPMAddPackage(
        NAME ImGui
        GITHUB_REPOSITORY ocornut/imgui
        GIT_TAG 8566fec661801a026e56f06cd53f5dac25c2595b
        DOWNLOAD_ONLY YES
)
set(ImGui_INCLUDE_DIR ${ImGui_SOURCE_DIR})

# Docking sources
set(ImGui_SOURCES
        ${ImGui_SOURCE_DIR}/imgui_internal.h
        ${ImGui_SOURCE_DIR}/imgui_tables.cpp
        ${ImGui_SOURCE_DIR}/imgui_widgets.cpp
        ${ImGui_SOURCE_DIR}/imgui_demo.cpp
        ${ImGui_SOURCE_DIR}/imgui.cpp
        ${ImGui_SOURCE_DIR}/imgui_draw.cpp
        ${ImGui_SOURCE_DIR}/imgui_widgets.cpp
        ${ImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        ${ImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${ImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        ${ImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${ImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
        ${ImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
        ${ImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${ImGui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        )
if(IOS)
    # Covers iOS implementation
elseif(APPLE)
    # Covers macOS implementation
    if(BEE_COMPILE_GLFW)
        list(APPEND ImGui_SOURCES
                ${ImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
                ${ImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
                )
    endif()
    if(BEE_COMPILE_WEBGPU)
        list(APPEND ImGui_SOURCES
                backends/imgui_impl_wgpu.cpp
                backends/imgui_impl_wgpu.h
                )
    endif()
elseif(ANDROID)
    # Covers Android implementation
elseif(WIN32)
    # Covers Windows implementation
    if(BEE_COMPILE_GLFW)
        list(APPEND ImGui_SOURCES
                ${ImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
                ${ImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
                )
    endif()
    if(BEE_COMPILE_WEBGPU)
        list(APPEND ImGui_SOURCES
                backends/imgui_impl_wgpu.cpp
                backends/imgui_impl_wgpu.h
                )
    endif()
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    if(BEE_COMPILE_GLFW)
        list(APPEND ImGui_SOURCES
                ${ImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
                ${ImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
                )
    endif()
    if(BEE_COMPILE_WEBGPU)
        list(APPEND ImGui_SOURCES
                backends/imgui_impl_wgpu.cpp
                backends/imgui_impl_wgpu.h
                )
    endif()
endif()

# Set library name
set(LIB_NAME ImGui)

# Define library
add_library(${LIB_NAME} STATIC ${ImGui_SOURCES})

if(IOS)
    # Covers iOS implementation
    if(BEE_COMPILE_WEBGPU)
        message("ImGui: Compiling with WebGPU support")
        enable_webgpu(${LIB_NAME})
    endif ()
elseif(APPLE)
    # Covers macOS implementation
    if(BEE_COMPILE_GLFW)
        target_link_libraries(${LIB_NAME} PUBLIC glfw)
    endif ()
    if(BEE_COMPILE_WEBGPU)
        message("ImGui: Compiling with WebGPU support")
        enable_webgpu(${LIB_NAME})
    endif ()
elseif(ANDROID)
    # Covers Android implementation
    if(BEE_COMPILE_WEBGPU)
        message("ImGui: Compiling with WebGPU support")
        enable_webgpu(${LIB_NAME})
    endif ()
elseif(WIN32)
    # Covers Windows implementation
    if(BEE_COMPILE_GLFW)
        target_link_libraries(${LIB_NAME} PUBLIC glfw)
    endif ()
    if(BEE_COMPILE_WEBGPU)
        message("ImGui: Compiling with WebGPU support")
        enable_webgpu(${LIB_NAME})
    endif ()
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    if(BEE_COMPILE_GLFW)
        target_link_libraries(${LIB_NAME} PUBLIC glfw)
    endif ()
    if(BEE_COMPILE_WEBGPU)
        message("ImGui: Compiling with WebGPU support")
        enable_webgpu(${LIB_NAME})
    endif ()
endif()

# Set include directories
target_include_directories(${LIB_NAME}
        PUBLIC ${ImGui_INCLUDE_DIR}
        PUBLIC ..SDL/include)

# Find GLFW
#find_package(glfw3 REQUIRED)
if(NOT IOS)
    find_package(Vulkan REQUIRED)
    target_link_libraries(${LIB_NAME} PRIVATE Vulkan::Vulkan)
endif()

target_link_libraries(${LIB_NAME} PRIVATE SDL3::SDL3)

# Enable C++11
target_compile_features(${LIB_NAME} PUBLIC cxx_std_11)

# Enable warnings
if(MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /W4)
else()
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
