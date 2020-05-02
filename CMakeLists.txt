cmake_minimum_required(VERSION 3.0)
project(Spacescape VERSION 0.7.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/Ogre)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/Qt)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif(MSVC)

find_package(OGRE 1.12 REQUIRED)

add_subdirectory(src/Spacescape)
add_subdirectory(src/SpacescapePlugin)
add_subdirectory(share)
