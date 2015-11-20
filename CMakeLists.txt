# @file     CMakeLists.txt
# @date     nov. 2015
# @author   PhRG / opticalp.fr
# @license  MIT

# minimalist cmake file
#  - create a very simple project
#  - look for Poco dependencies
#  - link to those dependencies

cmake_minimum_required (VERSION 2.8)
project ( Instrumentall )

# set search path for project specific cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# main sources
# custom parameters
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_BINARY_DIR}/bin" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/bin" )

## configure executable 

set ( MAIN_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src )
# include_directories ( ${MAIN_SOURCE_DIR} )

# list of source files
file (
  GLOB_RECURSE
  source_files
  "${MAIN_SOURCE_DIR}/*.cpp"
  "${MAIN_SOURCE_DIR}/*.ipp"
  "${MAIN_SOURCE_DIR}/*.h"
  )

add_executable (
  instrumentall
  ${source_files}
  )
  
add_definitions ( -DCMAKE_VERSION="${CMAKE_VERSION}" -DCOMPILER="${CMAKE_CXX_COMPILER_ID}" )

## configure dependencies

# find POCO
find_package( Poco 1.3.6 COMPONENTS Foundation Util REQUIRED )

message ( STATUS "found Poco version: ${Poco_VERSION}" )
message ( STATUS "libraries are: ${Poco_LIBRARIES}" ) 
    
if ( Poco_VERSION VERSION_GREATER "1.4.0" )
	add_definitions ( -DPOCO_VERSION_H )
endif ( )

target_link_libraries( instrumentall  Poco::Foundation Poco::Util )

# find python
# If you'd like to specify the installation of Python to use, you should
# modify the following cache variables:
#   PYTHON_LIBRARY             - path to the python library
#   PYTHON_INCLUDE_DIR         - path to where Python.h is found
find_package ( PythonLibs 2.7 EXACT )

if ( NOT PYTHONLIBS_FOUND ) 
    message ( WARNING "Python 2.7 not found" )
else ( NOT PYTHONLIBS_FOUND ) 
    add_definitions ( -DHAVE_PYTHON27 )
    message ( STATUS "Python libraries are: ${PYTHON_LIBRARIES}" )
    message ( STATUS "Python include path is: ${PYTHON_INCLUDE_DIRS}" )
    message ( STATUS "Python full version is: ${PYTHONLIBS_VERSION_STRING}" )
    include_directories ( ${PYTHON_INCLUDE_DIRS} )
    target_link_libraries ( instrumentall ${PYTHON_LIBRARIES} )
endif ( NOT PYTHONLIBS_FOUND ) 

## configure tests
enable_testing ()

# raw tests __without__ test framework
# test that the final binary is executable
add_test (
    NAME instruExec
    COMMAND $<TARGET_FILE:instrumentall>
    )
