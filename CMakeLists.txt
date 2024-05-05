cmake_minimum_required(VERSION 3.25)
include(cmake/UniversalAppleBuild.cmake)
include(src/Engine/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake)
project(
        BeeEngine_CPP
        DESCRIPTION "BeeEngine"
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BEEENGINE_DOTNET_VERSION "8.0")

include(cmake/StandardProjectSettings.cmake)
include(cmake/PackageInstaller.cmake)

# Link project_warnings as "library" to use the warnings
# specified in CompilerWarnings.cmake.
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

set(BEE_CLANG_FORMAT_FILE ${CMAKE_CURRENT_LIST_DIR}/.clang-format)
message(STATUS "Using clang-format style file: ${BEE_CLANG_FORMAT_FILE}")

macro(clang_format target sources)
        find_program(CLANG_FORMAT "clang-format")
        if(CLANG_FORMAT)
                message(STATUS "using clang-format on ${target}")
                #set(TARGET_NAME clang-format-${target})
                # Создание нового пустого списка для модифицированных путей
                set(modified_sources)

                # Добавление префикса к каждому элементу в исходном списке и сохранение результата в новом списке
                foreach(source ${sources})
                        list(APPEND modified_sources "${CMAKE_CURRENT_LIST_DIR}/${source}")
                endforeach()
                string(REPLACE ";" " " sources_string "${modified_sources}")
                set(clang_format_command "clang-format -i '-style=file:${BEE_CLANG_FORMAT_FILE}' ${sources_string}")
                #add_custom_target(
                #        ${TARGET_NAME}
                #        COMMAND ${clang_format_command}
                #)
                message(STATUS "clang-format command: ${clang_format_command}")
                #add_dependencies(${target} ${TARGET_NAME})
                # Выполнить clang-format непосредственно во время конфигурации
                execute_process(
                        COMMAND clang-format -i '-style=file:${BEE_CLANG_FORMAT_FILE}' ${sources_string}
                        RESULT_VARIABLE result
                        OUTPUT_VARIABLE clang_format_output
                        ERROR_VARIABLE clang_format_error
                )

                # Проверка результата выполнения
                if(NOT "${result}" STREQUAL "0")
                        message(WARNING "Clang-format reported an error: ${clang_format_error}")
                else()
                        message(STATUS "Clang-format output: ${clang_format_output}")
                endif()

        else()
                message(STATUS "clang-format not found")
        endif()
endmacro()

#include_directories(src/Engine/src/Allocator)
add_subdirectory(src/Engine)
add_subdirectory(src/Sandbox2D)
add_subdirectory(src/tests)
add_subdirectory(src/Editor)
add_subdirectory(src/LocalizationTool)
add_subdirectory(src/Runtime)

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




