cmake_minimum_required(VERSION 3.24)
project(BeeEngineEditor)

add_executable(BeeEngineEditor Start.cpp src/EditorApplication.cpp src/EditorApplication.h src/ConfigFile.cpp src/ConfigFile.h src/EditorLayer.cpp src/EditorLayer.h src/ViewPort.cpp src/ViewPort.h)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(Sandbox2D
        PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include/
        PUBLIC src
        )
target_link_libraries(${PROJECT_NAME} BeeEngine)