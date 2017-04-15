cmake_minimum_required(VERSION 3.7)

project("setup_sample" VERSION "1.2.3" LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(lib)


# create setup/package

#FIXME: verify if needed generator is installed
if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
	set(CPACK_GENERATOR "${CPACK_GENERATOR};WIX;NSIS")
	set(CPACK_WIX_UPGRADE_GUID "7ADE836B-A1A9-4B6E-90CE-2DF5BF3A1AFA")
else()
	set(CPACK_GENERATOR "${CPACK_GENERATOR};DEB;STGZ;TBZ2;TXZ;TZ;ZIP")

	# needed for .deb package	
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/license_debian.txt
		DESTINATION share/doc/${PROJECT_NAME}
		COMPONENT applications
		RENAME copyright
	)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/changelog.txt
		DESTINATION share/doc/${PROJECT_NAME}
		COMPONENT applications
		RENAME changelog
	)
endif()

set(CPACK_PACKAGE_VENDOR "MyGreatCompany")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Answer to the Ultimate Question")
set(CPACK_PACKAGE_DESCRIPTION "Answer to the Ultimate Question of Life, the Universe, and Everything")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "sample@mail.com")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}/${PROJECT_NAME}") # relevant for windows

SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")

SET(CPACK_STRIP_FILES TRUE)

# relevant for DEB packages
# unclear how to query automatically those packages
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "mantainer name  <${CPACK_PACKAGE_CONTACT}>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://www.example.com")




include(CPack)

