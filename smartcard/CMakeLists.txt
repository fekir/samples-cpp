cmake_minimum_required(VERSION 3.2)

project(smartcard)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(PkgConfig REQUIRED)
pkg_check_modules(PCSC REQUIRED libpcsclite )

message("ldflags:   ${PCSC_LDFLAGS}")
message("ldflags_o: ${PCSC_LDFLAGS_OTHER}")
message("include_d: ${PCSC_INCLUDE_DIRS}")
message("cflags:    ${PCSC_CFLAGS}")
message("cflags_o:  ${PCSC_CFLAGS_OTHER}")

include_directories(SYSTEM ${PCSC_INCLUDE_DIRS})


add_executable(${PROJECT_NAME} main.cpp)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PCSC_CFLAGS}")
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	LINK_FLAGS "${PCSC_LDFLAGS}"
)

set(SOURCE_FILES
	main.cpp
)
