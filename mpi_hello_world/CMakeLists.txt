cmake_minimum_required(VERSION 2.8)

project(MPIHelloWorld CXX)

find_package(MPI REQUIRED)

include_directories(${MPI_CXX_INCLUDE_PATH})

set(SOURCE_FILES )

add_executable(${PROJECT_NAME} 
	main.cpp
	${SOURCE_FILES}
)
target_link_libraries(${PROJECT_NAME} 
	${MPI_CXX_LIBRARIES}
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	COMPILE_FLAGS "${MPI_CXX_COMPILE_FLAGS}"
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	LINK_FLAGS "${MPI_CXX_LINK_FLAGS}"
)

