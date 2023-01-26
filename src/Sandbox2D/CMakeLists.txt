cmake_minimum_required(VERSION 3.0)
project(Sandbox2D)
add_executable(Sandbox2D src/main.cpp)

#add_subdirectory(../Engine/ zh)

target_include_directories(Sandbox2D
        PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include/
        PUBLIC src
        )
#[[target_link_directories(Sandbox2D
        PRIVATE src/
        PRIVATE ${CMAKE_SOURCE_DIR}/Engine/include/
        )]]
target_link_libraries(Sandbox2D BeeEngine)