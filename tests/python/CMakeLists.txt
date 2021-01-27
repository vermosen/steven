cmake_minimum_required (VERSION 3.13)

set(PYTHONPATH_TESTSUITE "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_SUFFIX}/python")
message(DEBUG "PYTHONPATH_TESTSUITE set to ${PYTHONPATH_TESTSUITE}")

add_test(
	NAME steven.python.package.import
	COMMAND ${PYTHON_EXECUTABLE} -c "from steven import *"
	WORKING_DIRECTORY ${TEST_FILE_DIR}
)

set_tests_properties(steven.python.package.import
  PROPERTIES 
	ENVIRONMENT "PYTHONPATH=${PYTHONPATH_TESTSUITE}"
)

add_subdirectory(time)