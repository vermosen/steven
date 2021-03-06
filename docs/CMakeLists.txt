cmake_minimum_required (VERSION 3.13)

# find doxygen
find_package(Doxygen REQUIRED)

if (DEFINED DOXYGEN_FOUND AND ${DOXYGEN_FOUND})
    message(STATUS "doxygen executable located at ${DOXYGEN_EXECUTABLE}")
else()
    message(FATAL_ERROR "cannot locate doxygen binary")
endif()

# doxygen configuration
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen  )
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in     )
set(DOXYFILE_TMP ${CMAKE_CURRENT_BINARY_DIR}/tmp/Doxyfile   )
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile       )

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

configure_file(
    ${DOXYFILE_IN}
    ${DOXYFILE_TMP}
    @ONLY
)

file(COPY ${DOXYFILE_TMP}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

add_custom_command(
    OUTPUT ${DOXYGEN_INDEX_FILE}
    DEPENDS steven
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT "Generating doxygen documentation"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(steven_doxygen_docs ALL DEPENDS ${DOXYGEN_INDEX_FILE})

# to visualize the doxygen doc, launch the server from build/docs/doxygen as
# python -m http.server -b X.Y.Z.Q

# generate the sphinx doc
add_subdirectory(sphinx)