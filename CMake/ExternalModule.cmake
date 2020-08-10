if(__EXTERNAL_MODULE_INCLUDED)
    return()
endif()
set(__EXTERNAL_MODULE_INCLUDED TRUE)

include(CMakeParseArguments)

macro(external_module)

    set(options "")
    set(oneValueArgs "HASH" "FILENAME")
    set(multiValueArgs "URL")
    cmake_parse_arguments(external_module "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT external_module_URL)
        message(FATAL_ERROR "Missing parameter: URL")
    endif()
    if(NOT external_module_FILENAME)
        message(FATAL_ERROR "Missing parameter: FILENAME")
    endif()

    if(NOT EXISTS "${CMAKE_BINARY_DIR}/${external_module_FILENAME}")
        if(external_module_HASH)
            set(dl_args "EXPECTED_HASH ${external_module_HASH}")
        endif()

        set(dl_successful FALSE)
        foreach(url ${external_module_URL})
            message(STATUS "Downloading ${external_module_FILENAME} from ${url}")

            file(DOWNLOAD "${url}" "${CMAKE_BINARY_DIR}/${external_module_FILENAME}" STATUS stat ${dl_args})
            list(GET stat 0 error_code)
            if(error_code)
                file(REMOVE "${CMAKE_BINARY_DIR}/${external_module_FILENAME}")
                list(GET status 1 error_message)
                message(WARNING "Failed to download ${external_module_FILENAME}: ${error_message}")
            else()
                set(dl_successful TRUE)
                break()
            endif()
        endforeach()

        if(NOT dl_successful)
            message(FATAL_ERROR "Failed to download ${external_module_FILENAME}")
        endif()

    endif()

    include(${CMAKE_BINARY_DIR}/${external_module_FILENAME})
endmacro()
