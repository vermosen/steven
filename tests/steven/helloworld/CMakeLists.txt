cmake_minimum_required (VERSION 3.13)

set(TARGET_NAME test_helloworld)

setup_component(TARGET ${TARGET_NAME})

file(GLOB_RECURSE TARGET_HEADERS "*.h")
file(GLOB_RECURSE TARGET_SOURCES "*.cpp")

add_executable(
    ${TARGET_NAME}
    ${TARGET_HEADERS}
    ${TARGET_SOURCES}
)

target_link_libraries(${TARGET_NAME}
	PUBLIC 
	steven
	CONAN_PKG::gtest 
	Threads::Threads
)

add_gtest(TARGET ${TARGET_NAME})
	