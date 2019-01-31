cmake_minimum_required (VERSION 3.6 )

project (CMakeVirtualenv )


find_package (Python3 COMPONENTS Interpreter Development )


if (NOT Python3_FOUND )

	## Find Python2
	find_package (PythonInterp 2.7 REQUIRED )

	set(CALL_PYTHON ${PYTHON_EXECUTABLE} )

else()

	set(CALL_PYTHON ${Python3_EXECUTABLE} )
endif()


find_program (VIRTUALENV virtualenv )

if (NOT VIRTUALENV )

    message (FATAL_ERROR "Could not find `virtualenv` in PATH" )
else()

	message (STATUS "Found `virtualenv`: ${VIRTUALENV}" )
endif()


set (VIRTUALENV ${VIRTUALENV} -p ${CALL_PYTHON} )


## Create and setup an up-to-date virtual python environment

set (ENV_NAME python-sandbox )
set (ENV_TGT  python-virtualenv )
set (ENV_DIR  "${CMAKE_BINARY_DIR}/${ENV_NAME}" )


## Setup ENV_DIR as virtual environment directory
add_custom_command(
	OUTPUT ${ENV_NAME}
	COMMAND ${CALL_PYTHON} -m virtualenv ${ENV_DIR}
	COMMENT "Create virtual python environment for m.css"
)


## Install requirements in ENV_DIR
add_custom_command(
    OUTPUT  ${ENV_NAME}_stamp
    DEPENDS ${ENV_NAME} requirements.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt ./requirements.txt
    COMMAND ./bin/pip install -r ./requirements.txt --upgrade
	WORKING_DIRECTORY ${ENV_DIR}
	COMMENT "Setup virtual python environment for m.css"
)


## Export target depending on virtual environment
add_custom_target (${ENV_TGT} DEPENDS ${ENV_NAME}_stamp )

## Export variables used in parent CMakeLists.txt
set (ENV_DIRECTORY ${ENV_DIR}              PARENT_SCOPE )
set (ENV_TARGET    ${ENV_TGT}              PARENT_SCOPE )
set (PYTHON_CMD    "${ENV_DIR}/bin/python" PARENT_SCOPE )

