cmake_minimum_required(VERSION 3.3)

project("sample_prog" VERSION "1.2.3" LANGUAGES CXX)

set(${PROJECT_NAME}_VERSION 1.2.3)

find_package(Doxygen REQUIRED)
include(doc/doxygen.cmake)
include(doc/dependencies.cmake)




add_library(sample_lib src/lib.cpp)
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} sample_lib)

create_documentation_with_doxygen(${PROJECT_NAME})

create_dependencies(${PROJECT_NAME})

