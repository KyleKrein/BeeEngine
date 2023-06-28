cmake_minimum_required(VERSION 3.24)
project(BeeEngine_CPP)
set(CMAKE_CXX_STANDARD 23)
#[[set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/Libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/Runtime)]]
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
include_directories(src/Engine/src/Allocator)

add_subdirectory(src/Sandbox2D)
add_subdirectory(src/tests)
add_subdirectory(src/Editor)
add_subdirectory(src/Engine)

#[[target_include_directories( ${PROJECT_NAME}
        PUBLIC Engine/src
        PUBLIC Sandbox2D
)
target_link_directories( ${PROJECT_NAME}
        PRIVATE Engine/src
        PRIVATE Sandbox2D
)

target_link_libraries( ${PROJECT_NAME}
        BeeEngine
)]]




