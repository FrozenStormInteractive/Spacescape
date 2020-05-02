find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(external/QtPropertyBrowser5)

INCLUDE_DIRECTORIES(../SpacescapePlugin/include/)

set(SPC_SOURCES src/Main.cpp
        src/QtSpacescapeAboutDialog.cpp
        src/QtSpacescapeExportFileDialog.cpp
        src/QtSpacescapeMainWindow.cpp
        src/QtSpacescapeWidget.cpp
        src/QtFilePathProperty.cpp
        src/aboutdialog.ui
        src/mainwindow.ui)

set(SPC_HEADERS include/QtSpacescapeAboutDialog.h
        include/QtSpacescapeAboutDialogUI.h
        include/QtSpacescapeExportFileDialog.h
        include/QtSpacescapeMainWindow.h
        include/QtSpacescapeUI.h
        include/QtSpacescapeWidget.h
        include/QtFilePathProperty.h)

IF(APPLE)
    list(APPEND SPC_SOURCES src/QtOgreWidgetOSX.cpp)
    list(APPEND SPC_HEADERS include/QtOgreWidgetOSX.h)
ELSE(APPLE)
    list(APPEND SPC_SOURCES src/QtOgreWidget.cpp)
    list(APPEND SPC_HEADERS include/QtOgreWidget.h)
ENDIF(APPLE)

set(SPC_RESOURCES Spacescape.qrc)

qt5_add_resources(SPC_SOURCES ${SPC_RESOURCES})

add_executable(Spacescape ${SPC_SOURCES} ${SPC_HEADERS} ${SPC_RESOURCES})

target_link_libraries(Spacescape SpacescapePlugin Qt5::Widgets QtPropertyBrowser OgreMain)
target_include_directories(Spacescape PUBLIC include)

file(GLOB_RECURSE MEDIA media/*)
source_group("media/materials/textures" FILES ${MEDIA})

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

install(TARGETS Spacescape RUNTIME DESTINATION .)
