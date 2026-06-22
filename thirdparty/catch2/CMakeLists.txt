## CMake file for fetching Catch2
## vim:fdm=marker

#cmake_minimum_required (VERSION 3.11 ) ## FetchContent available
cmake_minimum_required (VERSION 3.14 )  ## FetchContent_MakeAvailable available

find_package (Git REQUIRED )


## make Catch2 available

set (CATCH2_VERSION_TAG "v3.14.0" )
set (CATCH2_REPO_URL    "https://github.com/catchorg/Catch2.git" )

include (FetchContent )

FetchContent_Declare(
	catch2
	GIT_REPOSITORY ${CATCH2_REPO_URL}
	GIT_TAG        ${CATCH2_VERSION_TAG}
)
FetchContent_MakeAvailable (catch2 )

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE )

