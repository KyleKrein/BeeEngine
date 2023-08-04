cmake_minimum_required(VERSION 3.24)
project(BeeEngineEditor)

add_executable(BeeEngineEditor Start.cpp src/EditorApplication.cpp src/EditorApplication.h src/ConfigFile.cpp src/ConfigFile.h src/EditorLayer.cpp src/EditorLayer.h src/Panels/ViewPort.cpp src/Panels/ViewPort.h src/Panels/DockSpace.cpp src/Panels/DockSpace.h ../Engine/src/Gui/ImGui/IImGuiElement.h src/Panels/SceneHierarchyPanel.cpp src/Panels/SceneHierarchyPanel.h src/Panels/InspectorPanel.cpp src/Panels/InspectorPanel.h src/Panels/MenuBar.cpp src/Panels/MenuBar.h src/Panels/AssetPanel.cpp src/Panels/AssetPanel.h src/ProjectFile.cpp src/ProjectFile.h src/NativeScripting/ScriptParser.cpp src/NativeScripting/ScriptParser.h src/NativeScripting/ScriptScanner.cpp src/NativeScripting/ScriptScanner.h src/NativeScripting/CodeGenerators.cpp src/NativeScripting/CodeGenerators.h src/NativeScripting/GameBuilder.cpp src/NativeScripting/GameBuilder.h src/NativeScripting/StdioLogSystem.cpp src/VSProjectGeneration.cpp src/VSProjectGeneration.h)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(BeeEngineEditor
        PUBLIC ../Engine/include/
        PRIVATE ../Engine/vendor/
        PRIVATE vendor/FileWatcher
        PUBLIC src
        )

target_link_libraries(BeeEngineEditor PUBLIC BeeEngine)
copy_mono_dlls(BeeEngineEditor)
add_compile_definitions(BEE_ENABLE_CHECKS)
#file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})
file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Engine/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/EngineInclude)
file(COPY libs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Engine/vendor/mono/dotnet-libs/4.5 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/mono/lib/mono)
#file(COPY ../Engine/vendor/mono/dotnet-libs/4.5 DESTINATION ../lib/mono)
configure_file(${CMAKE_CURRENT_BINARY_DIR}/../Engine/BeeEngine.Core.dll ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.Core.dll COPYONLY)