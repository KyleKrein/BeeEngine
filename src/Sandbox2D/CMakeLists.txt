cmake_minimum_required(VERSION 3.25)
project(Sandbox2D)
set(SOURCE_FILES src/main.cpp
        src/VulkanTestLayer.cpp
        src/VulkanTestLayer.h
        #src/TestLayer.cpp
)
#if(APPLE AND NOT IOS)
#    add_executable(Sandbox2D MACOSX_BUNDLE ${SOURCE_FILES})
#    file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}/${PROJECT_NAME}.app/Contents/MacOS)
#    file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}/${PROJECT_NAME}.app/Contents/MacOS)
#else ()
    add_executable(Sandbox2D ${SOURCE_FILES})

#endif ()

#set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

#add_subdirectory(../Engine/ zh)

target_include_directories(Sandbox2D
        PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include/
        PUBLIC src
        )
#[[target_link_directories(Sandbox2D
        PRIVATE src/
        PRIVATE ${CMAKE_SOURCE_DIR}/Engine/include/
        )]]
target_link_libraries(Sandbox2D PUBLIC BeeEngine)

# until I finish developing the webgpu backend, these two lines will be here
target_include_directories( Sandbox2D PRIVATE ${webgpu_dawn_SOURCE_DIR}/include)
target_include_directories( Sandbox2D PRIVATE ${CMAKE_BINARY_DIR}/_deps/webgpu_dawn-build/gen/include)

file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})
file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})