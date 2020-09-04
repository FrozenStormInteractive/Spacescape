cmake_minimum_required(VERSION 3.0)
project(QtPropertyBrowser)

include(GenerateExportHeader)

message("${CMAKE_MODULE_PATH}")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

set(SOURCES src/QtPropertyBrowser.cpp
		src/QtPropertyManager.cpp
		src/QtEditorFactory.cpp
		src/QtVariantProperty.cpp
		src/QtTreePropertyBrowser.cpp
		src/QtButtonPropertyBrowser.cpp
		src/QtGroupBoxPropertyBrowser.cpp
		src/QtPropertyBrowserUtils.cpp)

set(HEADERS src/QtPropertyBrowser.h
		src/QtPropertyManager.h
		src/QtEditorFactory.h
		src/QtVariantProperty.h
		src/QtTreePropertyBrowser.h
		src/QtButtonPropertyBrowser.h
		src/QtGroupBoxPropertyBrowser.h
		src/QtPropertyBrowserUtils_p.h)

set(RESOURCES src/QtPropertyBrowser.qrc)
qt5_add_resources(SOURCES ${RESOURCES})

add_library(QtPropertyBrowser ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(QtPropertyBrowser PUBLIC src ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(QtPropertyBrowser PUBLIC Qt5::Widgets)

generate_export_header(QtPropertyBrowser)

if(BUILD_SHARED_LIBS)
	install(TARGETS QtPropertyBrowser RUNTIME DESTINATION .)
endif()
