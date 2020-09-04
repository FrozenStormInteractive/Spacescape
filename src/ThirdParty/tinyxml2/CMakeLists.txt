cmake_minimum_required(VERSION 3.0)
project(tinyxml2)

find_package(tinyxml2 QUIET)

if(tinyxml2_FOUND)

    if(NOT TARGET tinyxml2)
        add_library(tinyxml2 INTERFACE)
        if(tinyxml2_INCLUDE_DIRS)
            target_include_directories(tinyxml2 INTERFACE ${tinyxml2_INCLUDE_DIRS})
        endif()
        if(tinyxml2_LIBRARIES)
            target_link_libraries(tinyxml2 INTERFACE ${tinyxml2_LIBRARIES})
        endif()
        if(tinyxml2_COMPILE_DEFINITIONS)
            target_compile_definitions(tinyxml2 INTERFACE ${tinyxml2_COMPILE_DEFINITIONS})
        endif()
        if(tinyxml2_COMPILE_OPTIONS_LIST)
            target_compile_options(tinyxml2 INTERFACE ${tinyxml2_COMPILE_OPTIONS_LIST})
        endif()
    endif()

else()

    message(STATUS "tinyxml2 not found")
    message(STATUS "Building tinyxml2 from 3rdparty sources")

    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    add_library(tinyxml2 STATIC tinyxml2.cpp tinyxml2.h)

    set_target_properties(tinyxml2 PROPERTIES COMPILE_DEFINITIONS "TINYXML2_EXPORT")

    target_compile_definitions(tinyxml2 PUBLIC $<$<CONFIG:Debug>:TINYXML2_DEBUG>)
    target_include_directories(tinyxml2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    if(MSVC)
        target_compile_definitions(tinyxml2 PUBLIC -D_CRT_SECURE_NO_WARNINGS)
    endif(MSVC)

endif()


