cmake_minimum_required(VERSION 3.3)

project("sample_lib" LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED lib.cpp)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
