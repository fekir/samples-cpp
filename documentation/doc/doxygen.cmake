find_package(Doxygen REQUIRED)

function(create_documentation_with_doxygen target)

	# set variables use ind Doxyfile.in
	set(DOXY_PROJECT_NAME "${target}")
	set(DOXY_HAVE_DOT "${DOXYGEN_DOT_FOUND}")

	get_target_property(prop_version ${target} VERSIONS)
	set(DOXY_PROJECT_NUMBER "${prop_version}")
	
	get_target_property(prop_source_dir ${target} SOURCE_DIR)
	set(DOXY_SOURCE_DIR "${prop_source_dir}")

	set(doxyfile_in  ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
	set(doxyfile_out ${CMAKE_BINARY_DIR}/doc/${target}/Doxyfile)

	configure_file(${doxyfile_in} ${doxyfile_out})


	add_custom_target(DOXYGEN_${target}
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_out}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc/${target}
		COMMENT "Generating documentation with Doxygen"
		VERBATIM
	)

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_BINARY_DIR}/doc/${target}")

	install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html DESTINATION share/doc/${target})

endfunction()
