

if(WIN32)
	set(PROG_FILES_X86_ENV "PROGRAMFILES(X86)")
	set(PROG_FILES_ENV "ProgramW6432")
	find_program(CPPCHECK_EXECUTABLE
		NAMES cppcheck
		PATHS 
			"$ENV{${PROG_FILES_ENV}}/Cppcheck/"
			"$ENV{${PROG_FILES_X86_ENV}}/Cppcheck/"
	)
else()
	find_program(CPPCHECK_EXECUTABLE
		NAMES cppcheck
	)
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cppcheck REQUIRED_VARS CPPCHECK_EXECUTABLE)


# functions for adding targets with cppcheck

set(template [=[--template="[{severity}][{id}][{file}][line {line}] {message} {callstack}"]=])
# can it differ between targets??
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(template "gcc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(template "gcc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(template "vs")
endif()

# add option for overwriting template, std, library...

function(analyze_target_with_cppcheck target)
	# default values

	#set(prop_include "$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>")
	#set(additional_includes_param "-I$<JOIN:$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>, -I >")
	#set(additional_includes_param "$<$<BOOL:${prop_include}>:-I$<JOIN:${prop_include}, -I >>")

	# source files, may miss some headers that are used by the compiler
	#set(prop_sources "$<TARGET_PROPERTY:${target},SOURCES>")
	#set(sources "$<$<BOOL:${prop_sources}>:$<JOIN:${prop_sources},\;>>")
	get_target_property(sources ${target} SOURCES)

	# source directory, may contain some file that are not used by the compiler
	#set(prop_source_dir "$<TARGET_PROPERTY:${target},SOURCE_DIR>")
	get_target_property(prop_source_dir ${target} SOURCE_DIR)


	add_custom_target(CPPCHECK_${target}
		COMMAND "${CPPCHECK_EXECUTABLE}" "--verbose" "--template=${template}" "--enable=all" ${sources}
		WORKING_DIRECTORY ${prop_source_dir}  # does not take a generator expression...
		VERBATIM
	)
endfunction()



function(analyze_directory_with_cppcheck target_name directory)
	add_custom_target(${target_name}
		COMMAND "${CPPCHECK_EXECUTABLE}" "--verbose" "--template=${template}" "--enable=all" ${directory}
		VERBATIM
	)
endfunction()
