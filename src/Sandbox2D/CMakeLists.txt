cmake_minimum_required(VERSION 3.25)
project(Sandbox2D)
set(SOURCE_FILES src/main.cpp
  src/RmlUiLayer.cpp
  src/RmlUiLayer.hpp
        #src/TestLayer.cpp
)
    add_executable(Sandbox2D ${SOURCE_FILES})

target_include_directories(Sandbox2D
        PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include/
        PUBLIC src
        )
target_link_libraries(Sandbox2D PUBLIC BeeEngine)
file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})
file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})
