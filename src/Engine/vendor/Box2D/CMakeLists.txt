set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF CACHE STRING "DONT")
set(BOX2D_BUILD_DOCS OFF)
set(BOX2D_USER_SETTINGS OFF)
add_subdirectory(box2d)
add_library(Box2D INTERFACE)
target_link_libraries(Box2D INTERFACE box2d)
