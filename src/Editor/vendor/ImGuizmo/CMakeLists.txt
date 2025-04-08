cmake_minimum_required(VERSION 3.25)
project(ImGuizmo)

set(ImGuizmo_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/imguizmo)
set(IMGUIZMO_SOURCE_DIR ${ImGuizmo_SOURCE_DIR})

file(GLOB IMGUIZMO_SOURCES ${IMGUIZMO_SOURCE_DIR}/*.cpp)

file(GLOB IMGUIZMO_HEADERS ${IMGUIZMO_SOURCE_DIR}/*.h)

add_library(ImGuizmo STATIC ${IMGUIZMO_SOURCES} ${IMGUIZMO_HEADERS})

target_include_directories(ImGuizmo
        PUBLIC ${IMGUIZMO_SOURCE_DIR}
        )
target_link_libraries(ImGuizmo PRIVATE ImGui)
