

if(WIN32)
	set(PROG_FILES_X86_ENV "PROGRAMFILES(X86)")
	set(PROG_FILES_ENV "ProgramW6432")
	find_program(FLAWFINDER_EXECUTABLE
		NAMES flawfinder
		PATHS 
			"$ENV{${PROG_FILES_ENV}}/flawfinder/"
			"$ENV{${PROG_FILES_X86_ENV}}/flawfinder/"
			"C:/cygwin64/bin/"
			"C:/cygwin/bin/"
	)
	find_program(FLAWFINDER_PYTHON_EXECUTABLE
		NAMES python
		PATHS 
			"$ENV{${PROG_FILES_ENV}}/python/"
			"$ENV{${PROG_FILES_X86_ENV}}/python/"
			"C:/cygwin64/bin/"
			"C:/cygwin/bin/"
	)

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(flawfinder REQUIRED_VARS FLAWFINDER_EXECUTABLE FLAWFINDER_PYTHON_EXECUTABLE)
else()
	find_program(FLAWFINDER_EXECUTABLE
		NAMES flawfinder
	)
	
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(flawfinder REQUIRED_VARS FLAWFINDER_EXECUTABLE)
endif()



function(analyze_target_with_flawfinder target)
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

	if(WIN32)
		add_custom_target(FLAWFINDER_${target}
			COMMAND "${FLAWFINDER_PYTHON_EXECUTABLE}" "${FLAWFINDER_EXECUTABLE}" "${sources}"
			WORKING_DIRECTORY ${prop_source_dir}
			VERBATIM
		)
	else()
		add_custom_target(FLAWFINDER_${target}
			COMMAND "${FLAWFINDER_EXECUTABLE}" "${sources}"
			WORKING_DIRECTORY ${prop_source_dir}
			VERBATIM
		)
	endif()

endfunction()

function(analyze_directory_with_flawfinder target_name directory)
	set(OUT_DIR "${CMAKE_BINARY_DIR}/static_analysis")

	if(WIN32)
		add_custom_target(${target_name}
			COMMAND "${FLAWFINDER_PYTHON_EXECUTABLE}" "${FLAWFINDER_EXECUTABLE}" "${directory}"
			VERBATIM
		)
	else()
		add_custom_target(${target_name}
			COMMAND "${FLAWFINDER_EXECUTABLE}" "${directory}"
			VERBATIM
		)
	endif()

endfunction()
