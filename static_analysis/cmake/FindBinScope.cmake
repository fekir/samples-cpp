# https://www.microsoft.com/en-us/download/details.aspx?id=44995
# BinScope is a Microsoft verification tool that analyzes binaries on a project-wide level to ensure that they have been built in compliance with Microsoft’s Security Development Lifecycle (SDL) requirements and recommendations. 

if(WIN32)
	set(PROG_FILES_X86_ENV "PROGRAMFILES(X86)")
	set(PROG_FILES_ENV "ProgramW6432")
	find_program(BINSCOPE_EXECUTABLE
		NAMES binscope
		PATHS 
			"$ENV{${PROG_FILES_ENV}}/Microsoft BinScope 2014/"
			"$ENV{${PROG_FILES_X86_ENV}}/Microsoft BinScope 2014/"
	)
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(binscope REQUIRED_VARS CPPCHECK_EXECUTABLE)

function(analyze_binary_with_binscope target)
	get_target_property(prop_binary_dir ${target} BINARY_DIR)

	add_custom_target(BINSCOPE_${target}
		COMMAND "${BINSCOPE_EXECUTABLE}" "/nologo" "/outdir" "." $<TARGET_FILE:${PROJECT_NAME}>
		WORKING_DIRECTORY ${prop_binary_dir}
		VERBATIM
	)
endfunction()
