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
    include(GNUInstallDirs)
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
        if(inc MATCHES "^${PROJECT_SOURCE_ABS}")
            list(APPEND FILTERED_INCLUDES "${inc}")
	endif()
    endforeach()
    set(INTERFACE_INCLUDES ${FILTERED_INCLUDES})

    # Escape lists for use inside install(CODE)
    string(REPLACE ";" "|" INTERFACE_INCLUDES_ESCAPED "${INTERFACE_INCLUDES}")

    # Output directory will be: include/<target_name>
    install(CODE "
        include(GNUInstallDirs)
        message(STATUS \"Copying public headers for target: ${TARGET}\")

        set(INTERFACE_INCLUDES \"${INTERFACE_INCLUDES_ESCAPED}\")
        string(REPLACE \"|\" \";\" INTERFACE_INCLUDES \"\${INTERFACE_INCLUDES}\")

        foreach(INC_DIR \${INTERFACE_INCLUDES})
            if(IS_ABSOLUTE \${INC_DIR} AND EXISTS \${INC_DIR})
                execute_process(
                    COMMAND ${Python3_EXECUTABLE} \"${PY_SCRIPT_PATH}\" \"\${INC_DIR}\" \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${TARGET}\"
                    RESULT_VARIABLE result
                    OUTPUT_VARIABLE output
                    ERROR_VARIABLE error_output
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_STRIP_TRAILING_WHITESPACE
                )
                if(NOT result EQUAL 0)
                    message(FATAL_ERROR \"Failed to copy includes from \${INC_DIR}: \${error_output}\")
                else()
                    message(STATUS \"Copied includes from: \${INC_DIR} → \${CMAKE_INSTALL_INCLUDEDIR}/${TARGET}\")
                endif()
            else()
                message(WARNING \"Skipping non-existent or non-absolute path: \${INC_DIR}\")
            endif()
        endforeach()
    ")
endfunction()
