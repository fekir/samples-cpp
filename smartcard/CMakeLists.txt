cmake_minimum_required(VERSION 3.2)

project(smartcard)

set (CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME}
	pcsc.hpp
	main.cpp
)

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	add_definitions(-D_UNICODE -DUNICODE)
	target_link_libraries(${PROJECT_NAME} winscard)
else()
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(PCSC REQUIRED libpcsclite)

	include_directories(SYSTEM ${PCSC_INCLUDE_DIRS})
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
		LINK_FLAGS "${PCSC_LDFLAGS}"
	)
endif()
