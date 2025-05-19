## CMake script for installation
##
## vim:fdm=marker
##
## Prerequisistes from PARENT:
##
## - Variables: PROJECT_GEN_SOURCES_DIR, PROJECT_INCLUDE_INSTALL_SUBDIR,
##              PROJECT_NAME


## --- Variables

set (INSTALL_CMAKECONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}"
	CACHE PATH "Installation directory for cmake package files" )

set (PROJECT_EXPORT_SET "${PROJECT_NAME}-targets" )



## --- Install binary and exported header files

install (TARGETS ${PROJECT_NAME}
	EXPORT "${PROJECT_EXPORT_SET}"
	LIBRARY       DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	PUBLIC_HEADER DESTINATION
			"${CMAKE_INSTALL_PREFIX}/${PROJECT_INCLUDE_INSTALL_SUBDIR}" )

install (EXPORT ${PROJECT_EXPORT_SET}
	NAMESPACE libarcstk::
	DESTINATION "${INSTALL_CMAKECONFIG_DIR}" )



## --- Install pkgconfig and *.cmake files

if (NOT SKIP_INSTALL_FILES )


	## -- Support pkgconfig

	string (REPLACE lib ""   PKG_BIND_NAME "${PROJECT_NAME}" )

	configure_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/libarcstk.pc.in"
		"${PROJECT_GEN_SOURCES_DIR}/libarcstk.pc"
		@ONLY
	)

	unset (PKG_BIND_NAME )

	install (FILES "${PROJECT_GEN_SOURCES_DIR}/libarcstk.pc"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" )


	## -- Support CMake Packaging

	include (CMakePackageConfigHelpers )

	set (PROJECT_CONFIG_FILE "${PROJECT_NAME}-config.cmake" )

	set (PROJECT_CONFIG_VERSION_FILE "${PROJECT_NAME}-config-version.cmake")

	configure_package_config_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_CONFIG_FILE}.in"
		"${PROJECT_GEN_SOURCES_DIR}/${PROJECT_CONFIG_FILE}"
		INSTALL_DESTINATION "${INSTALL_CMAKECONFIG_DIR}"
	)

	write_basic_package_version_file (
		"${PROJECT_GEN_SOURCES_DIR}/${PROJECT_CONFIG_VERSION_FILE}"
		VERSION        ${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}
		COMPATIBILITY  SameMajorVersion
	)

	install (FILES
		"${PROJECT_GEN_SOURCES_DIR}/${PROJECT_CONFIG_FILE}"
		"${PROJECT_GEN_SOURCES_DIR}/${PROJECT_CONFIG_VERSION_FILE}"
		DESTINATION "${INSTALL_CMAKECONFIG_DIR}"
	)

endif ()

