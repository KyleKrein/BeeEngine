cmake_minimum_required(VERSION 3.24)
project(BeeEngine)
set(CMAKE_CXX_STANDARD 17)


add_library(BeeEngine STATIC src/BeeEngine.cpp src/Windowing/Window.cpp src/Windowing/Window.h src/Core/TypeDefines.h src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h)

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