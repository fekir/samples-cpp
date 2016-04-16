cmake_minimum_required(VERSION 2.8)


project(HelloWorld CXX)

set(SOURCE_FILES )

add_executable(${PROJECT_NAME} 
	main.cpp
	${SOURCE_FILES}
)
