CPMAddPackage(
        NAME tint
        GIT_REPOSITORY https://dawn.googlesource.com/tint
        GIT_TAG 45b139212530ea743cd08a2906953df026f3eedf
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