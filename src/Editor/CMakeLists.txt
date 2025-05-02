cmake_minimum_required(VERSION 3.24)
project(BeeEngineEditor)

add_executable(BeeEngineEditor Start.cpp src/EditorApplication.cpp src/EditorApplication.h src/ConfigFile.cpp src/ConfigFile.h src/EditorLayer.cpp src/EditorLayer.h src/Panels/ViewPort.cpp src/Panels/ViewPort.h src/Panels/DockSpace.cpp src/Panels/DockSpace.h ../Engine/src/Gui/ImGui/IImGuiElement.h src/Panels/SceneHierarchyPanel.cpp src/Panels/SceneHierarchyPanel.h src/Panels/InspectorPanel.cpp src/Panels/InspectorPanel.h src/Panels/MenuBar.cpp src/Panels/MenuBar.h src/Panels/ContentBrowserPanel.cpp src/Panels/ContentBrowserPanel.h src/ProjectFile.cpp src/ProjectFile.h src/VSProjectGeneration.cpp src/VSProjectGeneration.h
        src/AssetScanner.cpp
        src/AssetScanner.h
        src/Panels/AssetPanel.cpp
        src/Panels/AssetPanel.h
        src/Panels/AssetEditPanel.cpp
        src/Panels/AssetEditPanel.h
        src/ImGuiNativeDragAndDrop.h
        src/Panels/ImGuiUIEditor.h
        src/Panels/ImGuiUIEditor.cpp
        src/Panels/ProjectSettings.h
        src/Panels/ProjectSettings.cpp
        src/Panels/RmlUiViewport.cpp
        src/Panels/RmlUiViewport.hpp
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

if(NOT ${BEE_BUILD_BEEENGINE})
  find_library(BeeEngine_LIB BeeEngine REQUIRED)
  message("Found BeeEngine: ${BeeEngine_LIB}")
  get_filename_component(BeeEngineLibDir "${BeeEngine_LIB}" DIRECTORY)
  get_filename_component(BeeEnginePrefix "${BeeEngineLibDir}" DIRECTORY)
  set(BEE_CSHARP_LIBRARY_PATH ${BeeEnginePrefix}/share/BeeEngine/libs)
  add_library(BeeEngine STATIC IMPORTED GLOBAL)
  set_target_properties(BeeEngine PROPERTIES
    IMPORTED_LOCATION ${BeeEngine_LIB}
    INTERFACE_INCLUDE_DIRECTORIES ${BeeEnginePrefix}/include/BeeEngine
  )
  set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER>  <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> -Wl,--start-group -Wl,--whole-archive <LINK_LIBRARIES> -Wl,--no-whole-archive -Wl,--end-group")

endif()

target_include_directories(BeeEngineEditor
        PUBLIC src
        )

target_link_libraries(BeeEngineEditor 
  PUBLIC BeeEngine
)
file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${BEE_CSHARP_LIBRARY_PATH}/BeeEngine.Core.dll ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.Core.dll COPYONLY)
configure_file(${BEE_CSHARP_LIBRARY_PATH}/BeeEngine.NativeBridge.runtimeconfig.json ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.NativeBridge.runtimeconfig.json COPYONLY)
configure_file(${BEE_CSHARP_LIBRARY_PATH}/BeeEngine.NativeBridge.dll ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.NativeBridge.dll COPYONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/Localization/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Localization)
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Platforms/)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/Platforms/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Platforms)

set(BEE_EDITOR_INSTALL_PATH share/BeeEngineEditor)

install(TARGETS BeeEngineEditor DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/ DESTINATION ${BEE_EDITOR_INSTALL_PATH}/libs)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Localization/ DESTINATION ${BEE_EDITOR_INSTALL_PATH}/Localization)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Platforms/ DESTINATION ${BEE_EDITOR_INSTALL_PATH}/Platforms)

install(DIRECTORY ../Engine/Assets/Shaders/ DESTINATION ${BEE_EDITOR_INSTALL_PATH}/Shaders)
