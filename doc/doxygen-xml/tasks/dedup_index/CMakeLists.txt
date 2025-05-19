## CMake script for deduplicating doxygen's index.xml
##
## vim:fdm=marker

## In index.xml, every class in an inline namespace occurrs twice: once as
## member of the owning namespace where they are inlined to and a second time as
## members of the specific inline namespace. It did not find a way to suppress
## or avoid this.
## https://github.com/doxygen/doxygen/issues/5914
## https://github.com/doxygen/doxygen/pull/10070


message (STATUS "Doxygen Task: doxygen version requires index deduplication" )

