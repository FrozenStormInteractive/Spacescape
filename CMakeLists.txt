cmake_minimum_required(VERSION 3.0)

set(SPC_VERSION_MAJOR 0)
set(SPC_VERSION_MINOR 7)
set(SPC_VERSION_PATCH 0)

if(DEFINED SPC_VERSION_BUILD)
	set(SPC_VERSION ${SPC_VERSION_MAJOR}.${SPC_VERSION_MINOR}.${SPC_VERSION_PATCH}-${SPC_VERSION_BUILD})
else()
	set(SPC_VERSION ${SPC_VERSION_MAJOR}.${SPC_VERSION_MINOR}.${SPC_VERSION_PATCH})
endif()

project(Spacescape VERSION ${SPC_VERSION_MAJOR}.${SPC_VERSION_MINOR}.${SPC_VERSION_PATCH})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

include(ExternalModule)

option(SPC_USE_CONAN "" OFF)

if(SPC_USE_CONAN)
	external_module(FILENAME conan.cmake URL https://raw.githubusercontent.com/conan-io/cmake-conan/v0.15/conan.cmake)

	conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)
	conan_add_remote(NAME utopia URL https://api.bintray.com/conan/konradnotantoo/utopia)

	conan_cmake_run(BASIC_SETUP CONANFILE conanfile.txt BUILD missing)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
elseif(DEFINED CONANBUILD_DIR AND EXISTS "${CONANBUILD_DIR}/conanbuildinfo.cmake")
    include("${CONANBUILD_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
endif()

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif(MSVC)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/ThirdParty/Ogre)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/ThirdParty/Qt)
add_subdirectory(src/ThirdParty)
add_subdirectory(src/Spacescape)
add_subdirectory(src/SpacescapePlugin)
add_subdirectory(share)

set(CPACK_GENERATOR "STGZ" "TGZ" "ZIP")
if(WIN32 AND NOT UNIX)
	list(APPEND CPACK_GENERATOR "NSIS")
	set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/installer.ico")
	set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/packaging/windows/uninstaller.ico")
	set(CPACK_NSIS_HELP_LINK "https://github.com/FrozenStormInteractive/spacescape")
	set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/FrozenStormInteractive/spacescape")
elseif(UNIX)
	if(APPLE)
		list(APPEND CPACK_GENERATOR "DragNDrop")
	else()
		list(APPEND CPACK_GENERATOR "DEB" "RPM")
	endif()
endif()
set(CPACK_PACKAGE_VENDOR "FrozenStorm Interactive")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An awesome free & open-source tool for creating space skyboxes with stars and nebulas")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
	/dist
	/out
	/build
    /.*build.*
    /\\\\.DS_Store
)

include(CPack)
