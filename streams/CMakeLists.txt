cmake_minimum_required (VERSION 3.2)

project (streams CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(catch REQUIRED)
include_directories(SYSTEM ${CATCH_INCLUDE_DIRS})

add_executable (${PROJECT_NAME}
	main.cpp
        test.cpp
	streams.hpp
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
