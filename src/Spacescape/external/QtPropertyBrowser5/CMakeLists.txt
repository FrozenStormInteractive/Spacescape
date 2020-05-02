project(QtPropertyBrowser)

include(GenerateExportHeader)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

set(SOURCES src/qtpropertybrowser.cpp
		src/qtpropertymanager.cpp
		src/qteditorfactory.cpp
		src/qtvariantproperty.cpp
		src/qttreepropertybrowser.cpp
		src/qtbuttonpropertybrowser.cpp
		src/qtgroupboxpropertybrowser.cpp
		src/qtpropertybrowserutils.cpp)

set(HEADERS src/qtpropertybrowser.h
		src/qtpropertymanager.h
		src/qteditorfactory.h
		src/qtvariantproperty.h
		src/qttreepropertybrowser.h
		src/qtbuttonpropertybrowser.h
		src/qtgroupboxpropertybrowser.h
		src/qtpropertybrowserutils_p.h)

set(RESOURCES src/qtpropertybrowser.qrc)
qt5_add_resources(SOURCES ${RESOURCES})

add_library(QtPropertyBrowser ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(QtPropertyBrowser PUBLIC src ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(QtPropertyBrowser PUBLIC Qt5::Widgets)

generate_export_header(QtPropertyBrowser)

if(BUILD_SHARED_LIBS)
	install(TARGETS QtPropertyBrowser RUNTIME DESTINATION .)
endif()