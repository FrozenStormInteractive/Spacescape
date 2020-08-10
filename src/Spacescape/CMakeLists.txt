find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(external/QtPropertyBrowser5)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/SpacescapeVersion.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/SpacescapeVersion.h)

file(GLOB_RECURSE SPC_SOURCES src/*.cpp src/*.ui)

file(GLOB_RECURSE SPC_HEADERS include/*.h)

set(SPC_RESOURCES Spacescape.qrc)

qt5_add_resources(SPC_SOURCES ${SPC_RESOURCES})

if(WIN32)
    list(APPEND SPC_RESOURCES Spacescape.rc)
endif()

add_executable(Spacescape ${SPC_SOURCES} ${SPC_HEADERS} ${SPC_RESOURCES})

target_link_libraries(Spacescape SpacescapePlugin Qt5::Widgets QtPropertyBrowser OgreMain OgreBitesQt)
target_include_directories(Spacescape PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include)
set_target_properties(Spacescape PROPERTIES AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET Spacescape PROPERTY WIN32_EXECUTABLE true)
endif()

file(GLOB_RECURSE MEDIA media/*)
source_group("media/materials/textures" FILES ${MEDIA})

file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

IF(APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME Spacescape)
    set_target_properties(Spacescape PROPERTIES MACOSX_BUNDLE TRUE)

    set_source_files_properties(${MEDIA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/media/materials/textures)

    set(COPY_FRAMEWORKS ${OGRE_RenderSystem_GL_LIBRARY_REL} ${OGRE_FRAMEWORK_PATH} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}Release/Plugin_Spacescape.framework")
    source_group(Frameworks FILES ${COPY_FRAMEWORKS})

    file(GLOB COPY_RESOURCES app/mac/*.cfg)
    set_target_properties(Spacescape PROPERTIES RESOURCE "${COPY_RESOURCES}")
    set_source_files_properties("${COPY_RESOURCES}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(Spacescape PROPERTIES XCODE_ATTRIBUTE_OGRE_SDK_ROOT)
ENDIF()

install(TARGETS Spacescape
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})