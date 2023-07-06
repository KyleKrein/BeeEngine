set(USE_CPM ON CACHE BOOL "Use CPM to install dependencies")

if (USE_CPM)
    include(${CMAKE_CURRENT_LIST_DIR}/CPMInstaller.cmake)
else ()
    include(FetchContent)
endif ()

function(install_package package_name github_repo github_tag)
    find_package(${package_name} QUIET)
    if (NOT package_name_FOUND)
        message(STATUS "Installing ${package_name} from ${github_repo} ${github_tag}")
        if(USE_CPM)
            CPMAddPackage(
                    NAME ${package_name}
                    GITHUB_REPOSITORY ${github_repo}
                    GIT_TAG ${github_tag}
            )
        else()
            FetchContent_Declare(
                    ${package_name}
                    GIT_REPOSITORY ${github_repo}
                    GIT_TAG ${github_tag}
            )
            FetchContent_MakeAvailable(${package_name})
        endif ()
    else ()
        message(STATUS "Found ${package_name} ${${package_name}_VERSION}")
    endif ()
endfunction()