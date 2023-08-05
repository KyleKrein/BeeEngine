set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF)
set(BOX2D_BUILD_DOCS OFF)
set(BOX2D_USER_SETTINGS OFF)
CPMAddPackage(
        NAME Box2D
        GITHUB_REPOSITORY erincatto/Box2D
        GIT_TAG 411acc32eb6d4f2e96fc70ddbdf01fe5f9b16230
)

add_library(Box2D INTERFACE)
target_link_libraries(Box2D INTERFACE box2d)