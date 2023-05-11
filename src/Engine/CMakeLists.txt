cmake_minimum_required(VERSION 3.24)
project(BeeEngine)
set(CMAKE_CXX_STANDARD 17)


add_library(BeeEngine STATIC src/BeeEngine.cpp src/Core/TypeDefines.h src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h src/Windowing/WindowHandler/WindowHandler.cpp src/Windowing/WindowHandler/WindowHandler.h src/Windowing/WindowProperties.h src/Windowing/WindowHandler/GlfwWindowHandler.cpp src/Windowing/WindowHandler/GlfwWindowHandler.h src/Core/Logging/Log.h src/Core/Logging/Log.cpp src/Core/Events/Event.h src/Core/Events/EventQueue.cpp src/Core/Events/EventQueue.h src/Core/Layer.h src/Core/LayerStack.cpp src/Core/LayerStack.h src/Core/Input.cpp src/Core/Input.h src/KeyCodes.h src/Iterator.h src/TypeCase.h src/Core/Events/EventImplementations.h src/Windowing/VSync.h src/Renderer/Renderer.cpp src/Renderer/Renderer.h src/Renderer/RendererAPI.h src/Core/EntryPoint.cpp src/Platform/ImGui/ImGuiLoaderOpenGL.h src/Platform/ImGui/ImGuiController.h src/Core/Layer.cpp src/Platform/OpenGL/OpenGLRendererAPI.cpp src/Platform/OpenGL/OpenGLRendererAPI.h src/Renderer/RendererAPI.cpp)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/ImGui)

if(UNIX AND APPLE)
    add_compile_definitions(MACOS)
endif()
if(UNIX AND NOT APPLE)
    add_compile_definitions(LINUX)
endif()
if(WIN32)
    add_compile_definitions(WINDOWS)
endif()


target_include_directories( ${PROJECT_NAME}
        PUBLIC include
        PUBLIC vendor/glfw/include
        PUBLIC src
        PUBLIC vendor/glad/include
        PUBLIC vendor/spdlog/include
        PUBLIC vendor/debugbreak
        PUBLIC vendor/glm/glm
        PUBLIC vendor/ImGui
        )
target_link_directories( ${PROJECT_NAME}
        PRIVATE src
        PRIVATE vendor/glfw/include
        PRIVATE vendor/glad/include
        PRIVATE include
        PRIVATE vendor/spdlog/include
        PRIVATE vendor/debugbreak
        PRIVATE vendor/glm/glm
        PRIVATE vendor/ImGui
        )

target_link_libraries( ${PROJECT_NAME}
        glfw
        glad
        spdlog
        glm
        ImGui
        )