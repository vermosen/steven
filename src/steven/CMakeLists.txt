  
cmake_minimum_required (VERSION 3.13)

SET(TARGET_NAME steven)

setup_component(TARGET ${TARGET_NAME})

file(GLOB_RECURSE TARGET_HEADERS CONFIGURE_DEPENDS "*.h")
file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(
    ${TARGET_NAME} STATIC 
    ${TARGET_HEADERS}
    ${TARGET_SOURCES}
)

target_link_libraries(${TARGET_NAME}
	PUBLIC 
	CONAN_PKG::quantlib
	stdc++fs 
)

set_property(TARGET steven PROPERTY POSITION_INDEPENDENT_CODE ON)

install_library(TARGET ${TARGET_NAME})