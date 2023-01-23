cmake_minimum_required(VERSION 3.24)
project(BeeEngine_CPP)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Engine)
add_subdirectory(Sandbox2D)
add_executable(
        BeeEngine_CPP
        Sandbox2D/main.cpp)



target_include_directories( ${PROJECT_NAME}
        PUBLIC Engine/src
        PUBLIC Sandbox2D
)
target_link_directories( ${PROJECT_NAME}
        PRIVATE Engine/src
        PRIVATE Sandbox2D
)

target_link_libraries( ${PROJECT_NAME}
        BeeEngine
)




