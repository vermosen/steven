cmake_minimum_required (VERSION 3.13)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_yieldtermstructure
	CLASS test_termstructure 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_blackconstantvoltermstruture
	CLASS test_termstructure 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_schedule
	CLASS test_termstructure 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_fixedratebond
	CLASS test_termstructure 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_zerocouponbond
	CLASS test_termstructure 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_bondhelper
	CLASS test_termstructure 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)