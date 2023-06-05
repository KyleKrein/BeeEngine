# Настройка кода тестов
cmake_minimum_required(VERSION 3.24)
project(BeeEngine_Tests)



add_executable(BeeEngine_Tests tests_main.cpp GeneralPurposeAllocatorTest.cpp StringTests.cpp RenderingTests/TextureTest.cpp ApplicationInit.h "RenderingTests/GeneralRenderingTests.cpp" PropertyClassTest.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_subdirectory(vendor/gtest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


set(TEST_COMPILE_FLAGS "-DBEE_TEST_MODE")
target_compile_options(BeeEngine_Tests PUBLIC ${TEST_COMPILE_FLAGS})


target_link_libraries(BeeEngine_Tests
        gtest
        BeeEngine
        )

file(COPY AssetsForTests DESTINATION ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME})