install(DIRECTORY media/ DESTINATION media/)
install(DIRECTORY save/ DESTINATION save/)

if(WIN32)
	install(DIRECTORY app/win/ DESTINATION .)
elseif(APPLE)
	install(DIRECTORY app/win/ DESTINATION .)
elseif(UNIX)
	install(DIRECTORY app/linux/ DESTINATION .)
endif()


if(EXISTS ${OGRE_CONFIG_DIR}/plugins_d.cfg)
	file(COPY ${OGRE_CONFIG_DIR}/plugins_d.cfg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
elseif(EXISTS ${OGRE_CONFIG_DIR}/plugins_d.cfg)
	file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
