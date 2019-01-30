cmake_minimum_required (VERSION 3.6 )

project (CMakeVirtualenv )

#enable_testing()

find_package (Python3 COMPONENTS Interpreter Development )

if (NOT Python3_FOUND )

	## Find Python2
	find_package (PythonInterp 2.7 REQUIRED )
	## provides $PYTHON_EXECUTABLE

else()

	set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE} )
endif()


find_program (VIRTUALENV virtualenv )

if (NOT VIRTUALENV )

    message (FATAL_ERROR "Could not find `virtualenv` in PATH" )
else()

	message (STATUS "`virtualenv` was found: ${VIRTUALENV}" )
endif()


##set (VIRTUALENV ${VIRTUALENV} -p python2.7 )
set (VIRTUALENV ${VIRTUALENV} -p ${PYTHON_EXECUTABLE} )


## Setup an up-to-date virtual python environment

set (ENV_NAME venv )

add_custom_command(
	OUTPUT ${ENV_NAME}
	COMMAND ${PYTHON_EXECUTABLE} -m virtualenv ${CMAKE_BINARY_DIR}/${ENV_NAME}
)

add_custom_command(
    OUTPUT  ${ENV_NAME}.stamp
    DEPENDS ${ENV_NAME} requirements.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt ./requirements.txt
    COMMAND ./bin/pip install -r ./requirements.txt --upgrade
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${ENV_NAME}
)


## Add the setup to the target

add_custom_target (virtualenv ALL DEPENDS ${ENV_NAME} ${ENV_NAME}.stamp )


## Build command line to run py.test.

#set(PYTEST
#    ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python2
#    ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/py.test
#)


#add_custom_target(Tests ALL
#    DEPENDS venv.stamp
#    SOURCES requirements.txt
#)

#add_test(NAME run_tests
#    COMMAND ${PYTEST} ${CMAKE_CURRENT_SOURCE_DIR}/test_sample.py
#)
