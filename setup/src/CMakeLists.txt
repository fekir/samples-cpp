cmake_minimum_required(VERSION 3.3)

project("sample_prog" LANGUAGES CXX)


include_directories(../lib)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} "sample_lib")



install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
