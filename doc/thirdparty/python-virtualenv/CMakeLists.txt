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

    message (FATAL_ERROR "Could not find 'virtualenv' in $PATH" )
else()

	message (STATUS "Found 'virtualenv': ${VIRTUALENV}" )
endif()


set (VIRTUALENV ${VIRTUALENV} -p ${CALL_PYTHON} )


## Create and setup an up-to-date virtual python environment

set (ENV_NAME   python-sandbox )
set (ENV_DIR    "${CMAKE_BINARY_DIR}/${ENV_NAME}" )
set (ENV_TGT    python-virtualenv )
set (REQ_FILE   "requirements.txt" )
set (STAMP_FILE "update.stamp" )


## Setup ENV_DIR as virtual environment directory
add_custom_command(
	OUTPUT  "${ENV_DIR}"
	COMMAND ${CALL_PYTHON}
	ARGS    -m virtualenv "${ENV_DIR}"
	VERBATIM
	COMMENT "Create virtual python environment for m.css"
)


## Update and Install requirements file in ENV_DIR
add_custom_command(
	OUTPUT  "${ENV_DIR}/${REQ_FILE}"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${REQ_FILE}"
	COMMAND ${CMAKE_COMMAND}
	ARGS    -E copy
		"${CMAKE_CURRENT_SOURCE_DIR}/${REQ_FILE}"
		"${ENV_DIR}/${REQ_FILE}"
	VERBATIM
	COMMENT "Specify python requirements in virtual environment"
)


## Update and Install requirements dependencies in ENV_DIR
add_custom_command(
	OUTPUT  "${ENV_DIR}/${STAMP_FILE}"
	DEPENDS "${ENV_DIR}/${REQ_FILE}"
	COMMAND ./bin/pip install
	ARGS    -r "${ENV_DIR}/${REQ_FILE}" --upgrade
	COMMAND ${CMAKE_COMMAND}
	ARGS    -E copy "${ENV_DIR}/${REQ_FILE}" "${ENV_DIR}/${STAMP_FILE}"
	WORKING_DIRECTORY "${ENV_DIR}"
	VERBATIM
	COMMENT "Update python requirements in virtual environment"
)


## Export target depending on virtual environment
add_custom_target (${ENV_TGT} DEPENDS "${ENV_DIR}" "${ENV_DIR}/${STAMP_FILE}" )


## Export variables used in parent CMakeLists.txt
set (ENV_DIRECTORY "${ENV_DIR}"            PARENT_SCOPE )
set (ENV_TARGET    "${ENV_TGT}"            PARENT_SCOPE )
set (PYTHON_CMD    "${ENV_DIR}/bin/python" PARENT_SCOPE )

