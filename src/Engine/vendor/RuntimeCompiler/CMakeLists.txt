CPMAddPackage(
    NAME RuntimeCompiledCPlusPlus
    GITHUB_REPOSITORY RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus
    GIT_TAG 362caeb839202086abb2c430a8ed63c478a987e4
    DOWNLOAD_ONLY YES
)
set(BUILD_EXAMPLES OFF)

if(WIN32 AND NOT BEE_RUNTIMECPPCACHECHANGED)
    set(BEE_RUNTIMECPPCACHECHANGED ON CACHE INTERNAL "")
    message("Applying patches for RuntimeCompilerCPP for Windows")
    file(COPY
            ${CMAKE_CURRENT_LIST_DIR}/Patches/Compiler_PlatformWindows.cpp
            DESTINATION ${RuntimeCompiledCPlusPlus_SOURCE_DIR}/Aurora/RuntimeCompiler/
            )
    file(COPY
            ${CMAKE_CURRENT_LIST_DIR}/Patches/RuntimeObjectSystem_PlatformWindows.cpp
            DESTINATION ${RuntimeCompiledCPlusPlus_SOURCE_DIR}/Aurora/RuntimeObjectSystem
            )
endif ()

add_subdirectory(${RuntimeCompiledCPlusPlus_SOURCE_DIR}/Aurora ${CMAKE_CURRENT_BINARY_DIR}/CPPRuntime)

add_library(RuntimeCompiledCPP INTERFACE)
target_include_directories(RuntimeCompiledCPP
        INTERFACE ${RuntimeCompiledCPlusPlus_SOURCE_DIR}/Aurora/RuntimeObjectSystem
        INTERFACE ${RuntimeCompiledCPlusPlus_SOURCE_DIR}/Aurora/RuntimeCompiler
        )
target_link_libraries(RuntimeCompiledCPP INTERFACE RuntimeCompiler RuntimeObjectSystem)