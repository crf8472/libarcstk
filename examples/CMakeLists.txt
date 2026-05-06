## Build examples
## vim: fdm=marker

set (CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE )
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE  )

add_subdirectory ( "albumverify" )
add_subdirectory ( "albumcalc" )
add_subdirectory ( "albumid" )
add_subdirectory ( "dbarparse" )

