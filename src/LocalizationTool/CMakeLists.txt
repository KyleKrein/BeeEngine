cmake_minimum_required(VERSION 3.25)
project(
        LocalizationTool
        VERSION 0.1
        DESCRIPTION "A localization tool for translating text files, made for BeeEngine"
        LANGUAGES CXX C
)

add_executable(BeeLocalization Start.cpp src/LocalizationToolApplication.h src/LocalizationToolLayer.h src/LocalizationToolLayer.cpp)
set_property(TARGET BeeLocalization PROPERTY CXX_STANDARD 23)

target_include_directories(BeeLocalization PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(BeeLocalization PUBLIC BeeEngine)

add_compile_definitions(BEE_ENABLE_CHECKS)

file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})