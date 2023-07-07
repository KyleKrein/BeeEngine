cmake_minimum_required(VERSION 3.25)
include(cmake/UniversalAppleBuild.cmake)
project(
        BeeEngine_CPP
        DESCRIPTION "BeeEngine"
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/StandardProjectSettings.cmake)
include(cmake/PackageInstaller.cmake)

# Link project_warnings as "library" to use the warnings
# specified in CompilerWarnings.cmake.
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include_directories(src/Engine/src/Allocator)

add_subdirectory(src/Engine)
add_subdirectory(src/Sandbox2D)
add_subdirectory(src/tests)
add_subdirectory(src/Editor)

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




