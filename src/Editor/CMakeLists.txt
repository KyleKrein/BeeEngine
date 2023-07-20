cmake_minimum_required(VERSION 3.24)
project(BeeEngineEditor)

add_executable(BeeEngineEditor Start.cpp src/EditorApplication.cpp src/EditorApplication.h src/ConfigFile.cpp src/ConfigFile.h src/EditorLayer.cpp src/EditorLayer.h src/Panels/ViewPort.cpp src/Panels/ViewPort.h src/Panels/DockSpace.cpp src/Panels/DockSpace.h ../Engine/src/Gui/ImGui/IImGuiElement.h src/Panels/SceneHierarchyPanel.cpp src/Panels/SceneHierarchyPanel.h src/Panels/InspectorPanel.cpp src/Panels/InspectorPanel.h src/Panels/MenuBar.cpp src/Panels/MenuBar.h src/Panels/AssetPanel.cpp src/Panels/AssetPanel.h src/ProjectFile.cpp src/ProjectFile.h)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(Sandbox2D
        PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include/
        PUBLIC src
        )
target_link_libraries(${PROJECT_NAME} BeeEngine)
add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_CHECKS>)
#file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})
file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})