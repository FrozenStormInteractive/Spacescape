add_subdirectory(external/tinyxml2)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/SpacescapePluginVersion.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/SpacescapePluginVersion.h)

FILE(GLOB_RECURSE SPCPLG_SOURCES src/*.cpp)

FILE(GLOB_RECURSE SPCPLG_HEADERS include/*.h)

aux_source_directory(src/ SPCPLG_SOURCES)

add_library(SpacescapePlugin SHARED ${SPCPLG_SOURCES} ${SPCPLG_HEADERS})
target_include_directories(SpacescapePlugin PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include)
target_compile_definitions(SpacescapePlugin PUBLIC TIXML_USE_TICPP PRIVATE EXR_SUPPORT)
target_link_libraries(SpacescapePlugin OgreMain)

set_target_properties(SpacescapePlugin PROPERTIES OUTPUT_NAME "Plugin_Spacescape" PREFIX "")

IF(WIN32)
    target_compile_definitions(SpacescapePlugin PUBLIC OGRE_SPACESCAPEPLUGIN_EXPORTS)
ELSEIF(APPLE)
    SET_TARGET_PROPERTIES(SpacescapePlugin PROPERTIES VERSION ver1)
    SET_TARGET_PROPERTIES(SpacescapePlugin PROPERTIES FRAMEWORK TRUE)

    # Add the path where the Ogre framework was found
    if(NOT ${OGRE_FRAMEWORK_PATH} STREQUAL "")
      set_target_properties(SpacescapePlugin PROPERTIES
        COMPILE_FLAGS "-F${OGRE_FRAMEWORK_PATH}"
        LINK_FLAGS "-F${OGRE_FRAMEWORK_PATH}"
      )
    endif()
ENDIF(WIN32)

install(TARGETS SpacescapePlugin RUNTIME DESTINATION .)
