cmake_minimum_required(VERSION 3.0)
project(tinyxml2)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(tinyxml2 STATIC tinyxml2.cpp tinyxml2.h)

set_target_properties(tinyxml2 PROPERTIES
    COMPILE_DEFINITIONS "TINYXML2_EXPORT")

target_compile_definitions(tinyxml2 PUBLIC $<$<CONFIG:Debug>:TINYXML2_DEBUG>)
target_include_directories(tinyxml2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_definitions(tinyxml2 PUBLIC -D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)
