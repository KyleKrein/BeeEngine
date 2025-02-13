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
        src/Panels/ProjectSettings.cpp)
# files for native scripting: src/NativeScripting/ScriptParser.cpp src/NativeScripting/ScriptParser.h src/NativeScripting/ScriptScanner.cpp src/NativeScripting/ScriptScanner.h src/NativeScripting/CodeGenerators.cpp src/NativeScripting/CodeGenerators.h src/NativeScripting/GameBuilder.cpp src/NativeScripting/GameBuilder.h src/NativeScripting/StdioLogSystem.cpp
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_include_directories(BeeEngineEditor
        PUBLIC ../Engine/include/
        PRIVATE ../Engine/vendor/
        PRIVATE vendor/FileWatcher
        PUBLIC src
        )
target_link_libraries(BeeEngineEditor PUBLIC BeeEngine::WithScripting)
add_compile_definitions(BEE_ENABLE_CHECKS)
add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_PROFILING>)
add_compile_definitions($<$<CONFIG:Release>:BEE_ENABLE_PROFILING>)
#file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})
file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ../Engine/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/EngineInclude) #this is for NativeScripting
#file(COPY libs DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) #this is for NativeScripting

#file(COPY ../Engine/vendor/mono/dotnet-libs/4.5 DESTINATION ../lib/mono)
configure_file(${BEE_CSHARP_LIBRARY_PATH}/BeeEngine.Core.dll ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.Core.dll COPYONLY)
configure_file(${BEE_CSHARP_LIBRARY_PATH}/BeeEngine.NativeBridge.runtimeconfig.json ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.NativeBridge.runtimeconfig.json COPYONLY)
configure_file(${BEE_CSHARP_LIBRARY_PATH}/BeeEngine.NativeBridge.dll ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.NativeBridge.dll COPYONLY)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/Localization/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Localization)
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Platforms/)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/Platforms/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Platforms)
