cmake_minimum_required(VERSION 2.8)

project(CalculatePI CXX)

find_package(MPI REQUIRED)

include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})

add_executable(${PROJECT_NAME} 
	main.cpp
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

