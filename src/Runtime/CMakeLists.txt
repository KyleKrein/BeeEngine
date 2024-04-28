cmake_minimum_required(VERSION 3.29)
project(GameRuntime)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

add_executable(GameRuntime Start.cpp ${SOURCES})

set_property(TARGET GameRuntime PROPERTY CXX_STANDARD 23)

target_include_directories(GameRuntime PRIVATE "src")

target_link_libraries(GameRuntime PRIVATE BeeEngine::WithScripting)

configure_file(${CMAKE_CURRENT_BINARY_DIR}/../Engine/net${BEEENGINE_DOTNET_VERSION}/BeeEngine.Core.dll ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.Core.dll COPYONLY)
configure_file(${CMAKE_CURRENT_BINARY_DIR}/../Engine/net${BEEENGINE_DOTNET_VERSION}/BeeEngine.NativeBridge.runtimeconfig.json ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.NativeBridge.runtimeconfig.json COPYONLY)
configure_file(${CMAKE_CURRENT_BINARY_DIR}/../Engine/net${BEEENGINE_DOTNET_VERSION}/BeeEngine.NativeBridge.dll ${CMAKE_CURRENT_BINARY_DIR}/libs/BeeEngine.NativeBridge.dll COPYONLY)

file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_compile_definitions(BEE_ENABLE_CHECKS)
add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_PROFILING>)

include(InstallRequiredSystemLibraries)
install(TARGETS GameRuntime DESTINATION bin)
install(FILES "${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}" DESTINATION bin)
