# Настройка кода тестов
cmake_minimum_required(VERSION 3.24)
project(BeeEngine_Tests)
set(CMAKE_CXX_STANDARD 17)


add_executable(BeeEngine_Tests tests_main.cpp GeneralPurposeAllocatorTest.cpp)


add_subdirectory(vendor/gtest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


set(TEST_COMPILE_FLAGS "-DBEE_TEST_MODE")
target_compile_options(BeeEngine_Tests PUBLIC ${TEST_COMPILE_FLAGS})


target_link_libraries(BeeEngine_Tests
        gtest
        BeeEngine
        )