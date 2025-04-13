function(collect_all_public_includes target result)
    get_target_property(dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)
    if(dirs)
        list(APPEND includes ${dirs})
    endif()

    get_target_property(deps ${target} INTERFACE_LINK_LIBRARIES)
    foreach(dep IN LISTS deps)
        if(TARGET ${dep})
            collect_all_public_includes(${dep} child_includes)
            list(APPEND includes ${child_includes})
        endif()
    endforeach()

    # Удалить дубликаты
    list(REMOVE_DUPLICATES includes)
    set(${result} "${includes}" PARENT_SCOPE)
endfunction()


function(install_target_public_headers TARGET)
    include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/copy_headers.cmake)

    if(NOT TARGET ${TARGET})
        message(FATAL_ERROR "Target '${TARGET}' does not exist.")
    endif()

    collect_all_public_includes(${TARGET} INTERFACE_INCLUDES)

    if(NOT INTERFACE_INCLUDES)
        message(WARNING "Target '${TARGET}' has no PUBLIC or INTERFACE includes.")
        return()
    endif()
    get_filename_component(PROJECT_SOURCE_ABS "${CMAKE_SOURCE_DIR}" ABSOLUTE)
    set(FILTERED_INCLUDES "")

    foreach(inc IN LISTS INTERFACE_INCLUDES)
        set(resolved_inc "")
        if("${inc}" MATCHES "^\\$<BUILD_INTERFACE:([^>]+)>$")
            set(resolved_inc "${CMAKE_MATCH_1}")
        else()
            set(resolved_inc "${inc}")
        endif()

        if("${resolved_inc}" MATCHES "^${PROJECT_SOURCE_ABS}")
            list(APPEND FILTERED_INCLUDES "${resolved_inc}")
        else()
            message(STATUS "Ignoring external include: ${resolved_inc}")
        endif()
    endforeach()

    set(INTERFACE_INCLUDES ${FILTERED_INCLUDES})

    string(REPLACE ";" "|" INTERFACE_INCLUDES_ESCAPED "${INTERFACE_INCLUDES}")

    message(STATUS "Copying public headers for target: ${TARGET}")

    set(INTERFACE_INCLUDES "${INTERFACE_INCLUDES_ESCAPED}")
    string(REPLACE "|" ";" INTERFACE_INCLUDES "${INTERFACE_INCLUDES}")

    foreach(INC_DIR ${INTERFACE_INCLUDES})
        copy_headers(${INC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include)
    endforeach()

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION include/${TARGET})
endfunction()
