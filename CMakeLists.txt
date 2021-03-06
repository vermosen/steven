cmake_minimum_required (VERSION 3.13)

option(STEVEN-DOCS "create the sphinx documentation of the project" ON)
option(LLVM_TOOLING "add llvm tooling" OFF)

set(PROJECT_HOME "${CMAKE_CURRENT_LIST_DIR}")
set(PROJECT_NAME steven CACHE STRING "" FORCE)
set(PROJECT_NAMESPACE steven CACHE STRING "" FORCE)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_U)

message(STATUS "*** variable PROJECT_NAME set to: ${PROJECT_NAME} ***")
message(STATUS "*** variable PROJECT_NAMESPACE set to: ${PROJECT_NAMESPACE} ***")
message(STATUS "*** variable PROJECT_HOME set to: ${PROJECT_HOME} ***")

message(STATUS "*** retrieving cmake configuration from ${PROJECT_HOME}/cmake ***")

##############################
#      include toolsets      #
##############################

include("${PROJECT_HOME}/cmake/CMakeLists.txt")

###########################
#  defines cmake project  #
###########################

set(${PROJECT_NAME_U}_MAJOR_VERSION 0)
set(${PROJECT_NAME_U}_MINOR_VERSION 1)
set(${PROJECT_NAME_U}_BUILD_VERSION 1)

set(${PROJECT_NAME_U}_VERSION
  ${${PROJECT_NAME_U}_MAJOR_VERSION}.${${PROJECT_NAME_U}_MINOR_VERSION}.${${PROJECT_NAME_U}_BUILD_VERSION})

message(STATUS "project version set to ${${PROJECT_NAME_U}_VERSION}")

PROJECT(
	${PROJECT_NAME}
	LANGUAGES C CXX CUDA
	VERSION	${${PROJECT_NAME_U}_VERSION}
	DESCRIPTION "steven - a library to study Heston model"
)

##############################
# import thirdparty packages #
##############################

set(GTEST_VERS     "1.8.1" )
set(PYBIND11_VERS  "2.5.0" )
set(DOXYGEN_VERS   "1.8.18")
set(QUANTLIB_VERS  "1.22.0")
set(EIGEN_VERS     "3.3.7" )
set(META_VERS      "0.1.13")
set(IWYU_VERS      "0.15.0")

set(PACKAGES
   gtest
   pybind11
   doxygen
   quantlib
   eigen
	 meta
)

if (LLVM_TOOLING)
	set(PACKAGES ${PACKAGES} iwyu)
endif()

message(STATUS "package list has been set to ${PACKAGES}")

set(PKG_OPTIONS "quantlib:use_std_shared_ptr=True;quantlib:use_std_unique_ptr=True")

# imported packages
load_packages(
	NAME ${PACKAGES}
	USER ${CONAN_USERNAME}
	CHANNEL ${CONAN_CHANNEL}
	PROFILE ${CONAN_PROFILE}
	SETTINGS "build_type=${CMAKE_BUILD_TYPE}"
	OPTIONS "${PKG_OPTIONS}"
	UPDATE
)

#create_debug_conf(NAME ${PACKAGES})

message(DEBUG "CONAN_LIBS set to ${CONAN_LIBS}")

#####################################
#          add the sources          #
#####################################

include_directories(src)

#####################################
#     add the project libraries     #
#####################################

add_subdirectory(src/steven)

#####################################
#    add the project applications   #
#####################################

add_subdirectory(python/package)

#####################################
#   add the project documentation   #
#####################################

# breathe does not seem to work for now on ...
# TODO: compare to project 
# https://github.com/PCMSolver/pcmsolver
if (STEVEN-DOCS)
	add_subdirectory(docs)
endif()

#####################################
# add the test suite to the project #
#####################################

add_testsuite(DIRECTORY tests)