

if(WIN32)
	set(PROG_FILES_X86_ENV "PROGRAMFILES(X86)")
	set(PROG_FILES_ENV "ProgramW6432")
	find_program(RATS_EXECUTABLE
		NAMES rats
		PATHS 
			"$ENV{${PROG_FILES_ENV}}/rats/"
			"$ENV{${PROG_FILES_X86_ENV}}/rats/"
			"C:/cygwin64/bin/"
			"C:/cygwin/bin/"
	)
else()
	find_program(RATS_EXECUTABLE
		NAMES rats
	)
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(rats REQUIRED_VARS RATS_EXECUTABLE)


function(analyze_target_with_rats target)
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

	add_custom_target(RATS_${target}
		COMMAND "${RATS_EXECUTABLE}" "--warning" "1" "--html" ${sources}
		WORKING_DIRECTORY ${prop_source_dir}
		VERBATIM
	)
endfunction()

function(analyze_directory_with_rats target_name directory)
	set(OUT_DIR "${CMAKE_BINARY_DIR}/static_analysis")
	message( "${OUT_DIR}" )
	add_custom_target(${target_name}
			COMMAND "${RATS_EXECUTABLE}" "--warning" "1" "--html" "${directory}"
			VERBATIM
		)
endfunction()
