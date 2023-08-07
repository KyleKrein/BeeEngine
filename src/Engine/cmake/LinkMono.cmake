set(MONO_ROOT_DIR ${MONO_DIRECTORY})

if(IOS)
    # Covers iOS implementation
    message(FATAL_ERROR "iOS mono is not supported yet")
elseif(APPLE)
    # Covers macOS implementation
    string(APPEND MONO_ROOT_DIR "MacOS/")
elseif(ANDROID)
    # Covers Android implementation
    message(FATAL_ERROR "Android mono is not supported yet")
elseif(WIN32)
    # Covers Windows implementation
    string(APPEND MONO_ROOT_DIR "Windows/")
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    message(FATAL_ERROR "Linux mono is not supported yet")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    string(APPEND MONO_ROOT_DIR "Debug")
else ()
    string(APPEND MONO_ROOT_DIR "Release")
endif ()

set(MONO_ROOT_DIR_CACHE ${MONO_ROOT_DIR} CACHE INTERNAL "Mono root directory" FORCE)

function(enable_mono target)
    set(MONO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/mono/include)
    target_include_directories(${target} PUBLIC ${MONO_INCLUDE_DIR})
    if(IOS)
        # Covers iOS implementation
        message(FATAL_ERROR "iOS mono is not supported yet")
    elseif(APPLE)
        # Covers macOS implementation

    elseif(ANDROID)
        # Covers Android implementation
        message(FATAL_ERROR "Android mono is not supported yet")
    elseif(WIN32)
        # Covers Windows implementation
        target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/libmono-static-sgen.lib)
        target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/libgcmonosgen.lib)
        target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/libmini-sgen.lib)
        target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/libmonoruntime-sgen.lib)
        target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/libmonoutils.lib)
        target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/eglib.lib)
        #target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/mono-2.0-sgen.lib)
        #target_link_libraries(${target} PRIVATE ${MONO_ROOT_DIR}/MonoPosixHelper.lib)
    elseif (UNIX AND NOT APPLE)
        # Covers Linux implementation
        message(FATAL_ERROR "Linux mono is not supported yet")
    endif()
endfunction()

function(copy_mono_dlls target)
    if(IOS)
        # Covers iOS implementation
        message(FATAL_ERROR "iOS mono is not supported yet")
    elseif(APPLE)
        # Covers macOS implementation
        add_library(mono-2.0-sgen SHARED IMPORTED)
        set_target_properties(mono-2.0-sgen PROPERTIES IMPORTED_LOCATION ${MONO_ROOT_DIR_CACHE}/libmonosgen-2.0.dylib)
        set_target_properties(mono-2.0-sgen PROPERTIES IMPORTED_IMPLIB ${MONO_ROOT_DIR_CACHE}/libmonosgen-2.0.a)
        target_link_libraries(${target} PUBLIC mono-2.0-sgen)

        add_library(mono-nativ-compat SHARED IMPORTED)
        set_target_properties(mono-nativ-compat PROPERTIES IMPORTED_LOCATION ${MONO_ROOT_DIR_CACHE}/libmono-native-compat.dylib)
        set_target_properties(mono-nativ-compat PROPERTIES IMPORTED_IMPLIB ${MONO_ROOT_DIR_CACHE}/libmono-native-compat.a)
        target_link_libraries(${target} PUBLIC mono-nativ-compat)

        add_library(mono-nativ-unified SHARED IMPORTED)
        set_target_properties(mono-nativ-unified PROPERTIES IMPORTED_LOCATION ${MONO_ROOT_DIR_CACHE}/libmono-native-unified.dylib)
        set_target_properties(mono-nativ-unified PROPERTIES IMPORTED_IMPLIB ${MONO_ROOT_DIR_CACHE}/libmono-native-unified.a)
        target_link_libraries(${target} PUBLIC mono-nativ-unified)
    elseif(ANDROID)
        # Covers Android implementation
        message(FATAL_ERROR "Android mono is not supported yet")
    elseif(WIN32)
        # Covers Windows implementation
        add_library(mono-2.0-sgen SHARED IMPORTED)
        set_target_properties(mono-2.0-sgen PROPERTIES IMPORTED_LOCATION ${MONO_ROOT_DIR_CACHE}/mono-2.0-sgen.dll)
        set_target_properties(mono-2.0-sgen PROPERTIES IMPORTED_IMPLIB ${MONO_ROOT_DIR_CACHE}/mono-2.0-sgen.lib)
        target_link_libraries(${target} PUBLIC mono-2.0-sgen)
        add_library(MonoPosixHelper SHARED IMPORTED)
        set_target_properties(MonoPosixHelper PROPERTIES IMPORTED_LOCATION ${MONO_ROOT_DIR_CACHE}/MonoPosixHelper.dll)
        set_target_properties(MonoPosixHelper PROPERTIES IMPORTED_IMPLIB ${MONO_ROOT_DIR_CACHE}/MonoPosixHelper.lib)
        target_link_libraries(${target} PUBLIC MonoPosixHelper)
        #configure_file(${MONO_ROOT_DIR_CACHE}/mono-2.0-sgen.dll ${CMAKE_CURRENT_BINARY_DIR}/mono-2.0-sgen.dll COPYONLY)
        #configure_file(${MONO_ROOT_DIR_CACHE}/MonoPosixHelper.dll ${CMAKE_CURRENT_BINARY_DIR}/MonoPosixHelper.dll COPYONLY)
        #target_link_libraries(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/MonoPosixHelper.dll)
        #target_link_libraries(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/mono-2.0-sgen.dll)
    elseif (UNIX AND NOT APPLE)
        # Covers Linux implementation
        message(FATAL_ERROR "Linux mono is not supported yet")
    endif()
endfunction()