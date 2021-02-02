cmake_minimum_required (VERSION 3.13)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME test_create_american_option
	CLASS test_option 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME set_blackscholesmerton_process
	CLASS test_option 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME set_coxrossrubinstein_pricingengine
	CLASS test_option 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)

add_py_unittest(
	EXEC ${PYTHON_EXECUTABLE} 
	NAME find_implied_vol
	CLASS test_option 
	FILE test
	PYTHONPATH ${PYTHONPATH_TESTSUITE}
	DEPENDS steven.python.package.import
)