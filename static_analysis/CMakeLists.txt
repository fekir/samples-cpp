cmake_minimum_required(VERSION 3.3)

project("static_analysis" CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(Cppcheck REQUIRED)
analyze_target_with_cppcheck(${PROJECT_NAME})
analyze_directory_with_cppcheck(CPPCHECK_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})

find_package(Flawfinder REQUIRED)
analyze_target_with_flawfinder(${PROJECT_NAME})
analyze_directory_with_flawfinder(FLAWFINDER_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})

#find_package(Rats REQUIRED)
analyze_target_with_rats(${PROJECT_NAME})
analyze_directory_with_rats(RATS_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})

find_package(Pscan REQUIRED)
analyze_target_with_pscan(${PROJECT_NAME})
analyze_directory_with_pscan(PSCAN_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})


# https://github.com/L2Program/FlintPlusPlus
# add flintplusplus

# also gcc, clang and msvc can make diagnostics without compiling
