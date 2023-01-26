cmake_minimum_required(VERSION 3.24)
project(BeeEngine)
set(CMAKE_CXX_STANDARD 17)


add_library(BeeEngine STATIC src/BeeEngine.cpp src/Windowing/Window.cpp src/Windowing/Window.h src/Core/TypeDefines.h src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h src/Windowing/CrossPlatformWindow.h src/Windowing/WindowHandler/WindowHandler.cpp src/Windowing/WindowHandler/WindowHandler.h src/Windowing/WindowProperties.h src/Windowing/WindowHandler/GlfwWindowHandler.cpp src/Windowing/WindowHandler/GlfwWindowHandler.h src/Core/Logging/Log.h)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)



target_include_directories( ${PROJECT_NAME}
        PUBLIC include/
        PUBLIC vendor/glfw/include/
        PUBLIC src/
        PUBLIC vendor/glad/include/
        )
target_link_directories( ${PROJECT_NAME}
        PRIVATE src/
        PRIVATE vendor/glfw/include/
        PRIVATE vendor/glad/include/
        PRIVATE include/
        )

target_link_libraries( ${PROJECT_NAME}
        glfw
        glad
        )