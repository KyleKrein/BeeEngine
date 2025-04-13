set(PY_SCRIPT_PATH ${CMAKE_CURRENT_LIST_DIR}/copy_headers.py)
find_package(Python3 REQUIRED)
function(copy_headers SOURCE_DIR DEST_DIR)
  if(IS_ABSOLUTE ${SOURCE_DIR} AND EXISTS ${SOURCE_DIR})
                execute_process(
                    COMMAND ${Python3_EXECUTABLE} ${PY_SCRIPT_PATH} ${SOURCE_DIR} ${DEST_DIR}
                    RESULT_VARIABLE result
                    OUTPUT_VARIABLE output
                    ERROR_VARIABLE error_output
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_STRIP_TRAILING_WHITESPACE
                )
                if(NOT result EQUAL 0)
                    message(FATAL_ERROR "Failed to copy includes from ${SOURCE_DIR}: ${error_output}")
                else()
                    message(STATUS "Copied includes from: ${SOURCE_DIR} → ${DEST_DIR}")
                endif()
            else()
                message(WARNING "Skipping non-existent or non-absolute path: ${SOURCE_DIR}")
            endif()
endfunction()
