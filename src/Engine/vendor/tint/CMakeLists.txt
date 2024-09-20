CPMAddPackage(
        NAME tint
        GIT_REPOSITORY https://dawn.googlesource.com/tint
        GIT_TAG 3cb3f4a22006b39e08dfda6b1dc4d01fafb2f7a5
        DOWNLOAD_ONLY YES
)
#check if fetch_dawn_dependencies.py exists
if(NOT EXISTS ${tint_SOURCE_DIR}/tools/fetch_dawn_dependencies.py)
    file(COPY_FILE ${BeeEngine_SOURCE_DIR}/vendor/tint/patches/CMakeLists.txt ${tint_SOURCE_DIR}/third_party/CMakeLists.txt)
    file(COPY_FILE ${BeeEngine_SOURCE_DIR}/vendor/tint/patches/DEPS ${tint_SOURCE_DIR}/DEPS)
    file(COPY_FILE ${BeeEngine_SOURCE_DIR}/vendor/tint/patches/fetch_dawn_dependencies.py ${tint_SOURCE_DIR}/tools/fetch_dawn_dependencies.py)
endif ()
set(TINT_BUILD_SPV_READER ON)
set(TINT_BUILD_WGSL_READER OFF)
set(TINT_BUILD_SPV_WRITER ON)
set(TINT_BUILD_WGSL_WRITER OFF)
add_subdirectory(${tint_SOURCE_DIR} ${tint_BINARY_DIR} EXCLUDE_FROM_ALL)