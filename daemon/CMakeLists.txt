cmake_minimum_required (VERSION 3.2)

project (daemon CXX)

# should change to options, since the location may differ on other operating systems
set (CONFIG_DIR "/etc/daemon/")
set (SYSTEMD_DIR "/lib/systemd/system/")
set (BIN_DIR "/usr/bin/")

set(SRC_FILES
	daemon.hpp signals.hpp
)

set(CONF_FILES
	daemon.conf daemon.service
)

add_executable (${PROJECT_NAME}
	main.cpp
	${SRC_FILES}
	#config files
	${CONF_FILES}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)



install(TARGETS ${PROJECT_NAME}
	RUNTIME
	DESTINATION ${BIN_DIR}
)
install(FILES daemon.conf DESTINATION ${CONFIG_DIR} COMPONENT config)
install(FILES daemon.service DESTINATION ${SYSTEMD_DIR} COMPONENT config)

