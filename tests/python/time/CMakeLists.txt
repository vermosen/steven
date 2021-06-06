cmake_minimum_required (VERSION 3.13)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_set_date 
	CLASS test_time 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_get_date 
	CLASS test_time 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_print_date 
	CLASS test_time 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_us_calendar 
	CLASS test_time 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_calendar_advance
	CLASS test_time 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_act360_yearfraction
	CLASS test_time 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_convert_datetime
	CLASS test_time 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

