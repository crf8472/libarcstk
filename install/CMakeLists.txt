## CMake script for installation
##
## vim:fdm=marker
##
## Prerequisistes from PARENT:
##
## - Variables: LIBARCSTK_GENSRC_BINARY_DIR, LIBARCSTK_INCLUDE_INSTALL_SUBDIR,
##              PROJECT_NAME


## --- Variables

set (LIBARCSTK_INSTALL_CMAKECONFIG_DIR
	"${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}"
	CACHE PATH "Installation directory for cmake package files" )

set (LIBARCSTK_EXPORT_SET "${PROJECT_NAME}-targets" )



## --- Install binary and exported header files

install (TARGETS ${PROJECT_NAME}
	EXPORT "${LIBARCSTK_EXPORT_SET}"
	LIBRARY       DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	PUBLIC_HEADER DESTINATION
			"${CMAKE_INSTALL_PREFIX}/${LIBARCSTK_INCLUDE_INSTALL_SUBDIR}" )

install (EXPORT ${LIBARCSTK_EXPORT_SET}
	NAMESPACE libarcstk::
	DESTINATION "${LIBARCSTK_INSTALL_CMAKECONFIG_DIR}" )



## --- Install pkgconfig and *.cmake files

if (NOT SKIP_INSTALL_FILES )


	## -- Support pkgconfig

	string (REPLACE lib "" LIBARCSTK_PKG_BIND_NAME "${PROJECT_NAME}" )

	configure_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/libarcstk.pc.in"
		"${LIBARCSTK_GENSRC_BINARY_DIR}/libarcstk.pc"
		@ONLY
	)

	unset (LIBARCSTK_PKG_BIND_NAME )

	install (FILES "${LIBARCSTK_GENSRC_BINARY_DIR}/libarcstk.pc"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" )


	## -- Support CMake Packaging

	include (CMakePackageConfigHelpers )

	set (LIBARCSTK_CONFIG_FILE "${PROJECT_NAME}-config.cmake" )

	set (LIBARCSTK_CONFIG_VERSION_FILE "${PROJECT_NAME}-config-version.cmake")

	configure_package_config_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/${LIBARCSTK_CONFIG_FILE}.in"
		"${LIBARCSTK_GENSRC_BINARY_DIR}/${LIBARCSTK_CONFIG_FILE}"
		INSTALL_DESTINATION "${LIBARCSTK_INSTALL_CMAKECONFIG_DIR}"
	)

	write_basic_package_version_file (
		"${LIBARCSTK_GENSRC_BINARY_DIR}/${LIBARCSTK_CONFIG_VERSION_FILE}"
		VERSION        ${PROJECT_VERSION}
		COMPATIBILITY  SameMajorVersion
	)

	install (FILES
		"${LIBARCSTK_GENSRC_BINARY_DIR}/${LIBARCSTK_CONFIG_FILE}"
		"${LIBARCSTK_GENSRC_BINARY_DIR}/${LIBARCSTK_CONFIG_VERSION_FILE}"
		DESTINATION "${LIBARCSTK_INSTALL_CMAKECONFIG_DIR}"
	)

endif ()

