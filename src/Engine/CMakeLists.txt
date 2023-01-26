cmake_minimum_required(VERSION 3.24)
project(BeeEngine)
set(CMAKE_CXX_STANDARD 17)


add_library(BeeEngine STATIC src/BeeEngine.cpp src/Windowing/Window.cpp src/Windowing/Window.h src/Core/TypeDefines.h src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h src/Windowing/CrossPlatformWindow.h src/Windowing/WindowHandler/WindowHandler.cpp src/Windowing/WindowHandler/WindowHandler.h src/Windowing/WindowProperties.h src/Windowing/WindowHandler/GlfwWindowHandler.cpp src/Windowing/WindowHandler/GlfwWindowHandler.h src/Core/Logging/Log.h src/Core/Logging/Log.cpp src/Core/Events/Event.h src/Core/Events/EventQueue.cpp src/Core/Events/EventQueue.h src/Core/Layer.h src/Core/LayerStack.cpp src/Core/LayerStack.h src/Core/Input.cpp src/Core/Input.h src/KeyCodes.h src/Iterator.h src/TypeCase.h src/Core/Events/EventImplementations.h src/Windowing/VSync.h)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/spdlog)



target_include_directories( ${PROJECT_NAME}
        PUBLIC include
        PUBLIC vendor/glfw/include
        PUBLIC src
        PUBLIC vendor/glad/include
        PUBLIC vendor/spdlog/include
        PUBLIC vendor/debugbreak
        )
target_link_directories( ${PROJECT_NAME}
        PRIVATE src
        PRIVATE vendor/glfw/include
        PRIVATE vendor/glad/include
        PRIVATE include
        PRIVATE vendor/spdlog/include
        PRIVATE vendor/debugbreak
        )

target_link_libraries( ${PROJECT_NAME}
        glfw
        glad
        spdlog
        )