
if(WIN32)
	set(PROG_FILES_X86_ENV "PROGRAMFILES(X86)")
	set(PROG_FILES_ENV "ProgramW6432")
	find_program(PSCAN_EXECUTABLE
		NAMES pscan
		PATHS 
			"$ENV{${PROG_FILES_ENV}}/pscan/"
			"$ENV{${PROG_FILES_X86_ENV}}/pscan/"
			"C:/cygwin64/bin/"
			"C:/cygwin/bin/"
	)
else()
	find_program(PSCAN_EXECUTABLE
		NAMES pscan
	)
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(pscan REQUIRED_VARS PSCAN_EXECUTABLE)


function(analyze_target_with_pscan target)
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

	# need a temporary -cmake file since pscan returns != 0 if it encounters same possible errors
	file(
		GENERATE OUTPUT ${CMAKE_BINARY_DIR}/PSCAN_${target}.cmake
		CONTENT
		"execute_process(COMMAND ${PSCAN_EXECUTABLE} ${sources})"
	)

	add_custom_target(PSCAN_${target}
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/PSCAN_${target}.cmake
		WORKING_DIRECTORY ${prop_source_dir}
		VERBATIM
	)
endfunction()

function(analyze_directory_with_pscan target_name directory)
	file(GLOB_RECURSE sources "${directory}/*.c*" "${directory}/*.h*")
	
		# need a temporary -cmake file since pscan returns != 0 if it encounters same possible errors
	file(
		GENERATE OUTPUT ${CMAKE_BINARY_DIR}/${target_name}.cmake
		CONTENT
		"execute_process(COMMAND ${PSCAN_EXECUTABLE} ${sources})"
	)

	add_custom_target(${target_name}
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/${target_name}.cmake
		WORKING_DIRECTORY ${prop_source_dir}
		VERBATIM
	)
endfunction()
