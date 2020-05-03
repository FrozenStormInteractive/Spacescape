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
	install(FILES ${OGRE_CONFIG_DIR}/plugins_d.cfg DESTINATION .)
endif()

if(EXISTS ${OGRE_CONFIG_DIR}/plugins.cfg)
	install(FILES ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION .)
endif()
