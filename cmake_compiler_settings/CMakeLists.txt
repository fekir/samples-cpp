cmake_minimum_required(VERSION 3.3)

project("compiler_settings" CXX)

##################################################################
# cmake settings
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	include(windows_flags)
endif()

if     ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	include(msvc_flags)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	include(gcc_flags)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	include(clang_flags)
endif()

add_executable(${PROJECT_NAME} main.cpp)

# set standard, without extensions
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

# or do it per-target
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)
