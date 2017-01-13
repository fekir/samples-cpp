cmake_minimum_required(VERSION 3.3)

project("compiler_settings" CXX)

##################################################################
# cmake settings
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	include(windows_flags)
endif()
if(MSVC)
	include(msvc_flags)
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
	include(gcc_flags)
endif()

add_executable(${PROJECT_NAME} main.cpp)
