## CMake script for installation
## vim:fdm=marker


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



## --- Install *.cmake packaging files

if (NOT SKIP_INSTALL_FILES )

	include (CMakePackageConfigHelpers )

	set (LIBARCSTK_CONFIG_FILE "${PROJECT_NAME}-config.cmake" )

	set (LIBARCSTK_CONFIG_VERSION_FILE "${PROJECT_NAME}-config-version.cmake")

	configure_package_config_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/${LIBARCSTK_CONFIG_FILE}.in"
		"${CMAKE_CURRENT_BINARY_DIR}/${LIBARCSTK_CONFIG_FILE}"
		INSTALL_DESTINATION "${LIBARCSTK_INSTALL_CMAKECONFIG_DIR}"
	)

	write_basic_package_version_file (
		"${CMAKE_CURRENT_BINARY_DIR}/${LIBARCSTK_CONFIG_VERSION_FILE}"
		VERSION        ${PROJECT_VERSION}
		COMPATIBILITY  SameMajorVersion
	)

	install (FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${LIBARCSTK_CONFIG_FILE}"
		"${CMAKE_CURRENT_BINARY_DIR}/${LIBARCSTK_CONFIG_VERSION_FILE}"
		DESTINATION "${LIBARCSTK_INSTALL_CMAKECONFIG_DIR}"
	)

endif ()



## --- pkgconfig

if (NOT SKIP_INSTALL_FILES )

	add_subdirectory (pkgconfig )

endif ()

