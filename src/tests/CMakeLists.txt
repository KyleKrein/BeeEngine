# Настройка кода тестов
cmake_minimum_required(VERSION 3.24)
project(BeeEngine_Tests)



add_executable(BeeEngine_Tests tests_main.cpp GeneralPurposeAllocatorTest.cpp StringTests.cpp RenderingTests/TextureTest.cpp ApplicationInit.h "RenderingTests/GeneralRenderingTests.cpp" PropertyClassTest.cpp TestFileDialogs.cpp CodeSafetyCheck.cpp
        ThreadPoolTest.cpp
        TypeDefinesTests.cpp
        FormatStringTest.cpp
        TransformTests.cpp
        HashTests.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

add_subdirectory(vendor/gtest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


set(TEST_COMPILE_FLAGS "-DBEE_TEST_MODE")
target_compile_options(BeeEngine_Tests PUBLIC ${TEST_COMPILE_FLAGS})
add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_CHECKS>)
add_compile_definitions($<$<CONFIG:Debug>:BEE_CUSTOM_MAIN>)
add_compile_definitions($<$<CONFIG:Release>:BEE_CUSTOM_MAIN>)
add_compile_definitions($<$<CONFIG:Release>:BEE_ENABLE_CHECKS>)

add_compile_definitions(BEE_ENABLE_CHECKS)
add_compile_definitions(BEE_CUSTOM_MAIN)
target_link_libraries(BeeEngine_Tests
        PUBLIC gtest
        PUBLIC BeeEngine
)

set(MONO_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../Engine/vendor/mono/libs/)
configureMono(${MONO_DIRECTORY})
copy_mono_dlls(BeeEngine_Tests)

file(COPY AssetsForTests DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})

file(COPY ../Engine/Assets/Shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})