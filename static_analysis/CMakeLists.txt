cmake_minimum_required(VERSION 3.3)

project("static_analysis" CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if(MSVC)
    add_compile_options(/GS-)
endif()

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(Cppcheck)
if(CPPCHECK_FOUND)
	analyze_target_with_cppcheck(${PROJECT_NAME})
	analyze_directory_with_cppcheck(CPPCHECK_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})
endif()

find_package(Flawfinder)
if(FLAWFINDER_FOUND)
	analyze_target_with_flawfinder(${PROJECT_NAME})
	analyze_directory_with_flawfinder(FLAWFINDER_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})
endif()

find_package(Rats)
if(RATS_FOUND)
	analyze_target_with_rats(${PROJECT_NAME})
	analyze_directory_with_rats(RATS_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})
endif()

find_package(Pscan)
if(PSCAN_FOUND)
	analyze_target_with_pscan(${PROJECT_NAME})
	analyze_directory_with_pscan(PSCAN_2_${PROJECT_NAME} ${CMAKE_SOURCE_DIR})
endif()

find_package(BinScope)
if(BINSCOPE_FOUND)
	analyze_binary_with_binscope(${PROJECT_NAME})
endif()

find_package(vera++) # apparently does not define VERA++_FOUND
if(VERA++_EXECUTABLE)

	# taken from https://bitbucket.org/verateam/vera/wiki/Running, but does not work on my machine
	#include(${VERA++_USE_FILE})
	#add_vera_targets(*.h *.hpp *.c *.cpp
	#	RECURSE
	#	ROOT "${CMAKE_SOURCE_DIR}/src"
	#)

	get_target_property(sources ${PROJECT_NAME} SOURCES)
	get_target_property(prop_source_dir ${PROJECT_NAME} SOURCE_DIR)
	add_custom_target(VERA++_${PROJECT_NAME} vera++ "--show-rule" "--warning" "${sources}" WORKING_DIRECTORY "${prop_source_dir}")
endif()

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(iwyu_path)
	# automatically executed when building project, not separate target
	set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()



# https://github.com/L2Program/FlintPlusPlus
# add flintplusplus

# also gcc, clang and msvc can make diagnostics without compiling
