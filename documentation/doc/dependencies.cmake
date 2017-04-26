# search for dot
find_program (DOT_EXE NAMES dot REQUIRED)

function(create_dependencies target)

	add_custom_target(DEPENDENCIES_${target}
		COMMAND "cmake" "--graphviz=doc/${target}/dependencies.dot" "${CMAKE_BINARY_DIR}"
		COMMAND "${DOT_EXE}" "-Tpng" "${CMAKE_BINARY_DIR}/doc/${target}/dependencies.dot" "-o" "${CMAKE_BINARY_DIR}/doc/${target}/dependencies.png"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		VERBATIM
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_BINARY_DIR}/doc/${target}")

	install(FILES ${CMAKE_BINARY_DIR}/doc/${target}/dependencies.png DESTINATION share/doc/${target}/)

endfunction()
