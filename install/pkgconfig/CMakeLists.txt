## CMake script for pkgconfig support
## vim:fdm=marker

## --- Configure + install libarcstk.pc

string (REPLACE lib ""   LIBARCSTK_PKG_BIND_NAME "${PROJECT_NAME}" )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/libarcstk.pc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/libarcstk.pc"
	@ONLY
)

unset (LIBARCSTK_PKG_BIND_NAME )

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/libarcstk.pc"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" )

# 1}}}

