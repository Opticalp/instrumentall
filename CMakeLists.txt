# @file     CMakeLists.txt
# @date     nov. 2015
# @author   PhRG / opticalp.fr
# @license  MIT

# minimalist cmake file
#  - create a very simple project
#  - look for Poco dependencies
#  - link to those dependencies

cmake_minimum_required (VERSION 2.8.8)
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
include_directories ( ${MAIN_SOURCE_DIR} )

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
  
add_definitions ( -DCMAKE_VERSION="${CMAKE_VERSION}" -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER_ID}" )

if (MSVC)
  add_definitions ( -DNOMINMAX )
endif (MSVC)

## configure dependencies

find_package ( Threads REQUIRED )

target_link_libraries(
    instrumentall      
    ${CMAKE_THREAD_LIBS_INIT}
    )    

# find POCO
find_package( Poco 1.3.6 COMPONENTS Foundation Util REQUIRED )

message ( STATUS "found Poco version: ${Poco_VERSION}" )
if ( DEFINED Poco_LIBRARIES )
    message ( STATUS "libraries are: ${Poco_LIBRARIES}" )
endif ( DEFINED Poco_LIBRARIES )
    
if ( Poco_VERSION VERSION_GREATER "1.4.0" )
	add_definitions ( -DPOCO_VERSION_H )
endif ( )

target_link_libraries( instrumentall  Poco::Foundation Poco::Util )

## generate build info

ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/versionBuild/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version/version.h
                     -D VERSION_FILE=${CMAKE_SOURCE_DIR}/VERSION
                     -D FORK_NAME_FILE=${CMAKE_SOURCE_DIR}/fork_NAME
                     -D FORK_VERSION_FILE=${CMAKE_SOURCE_DIR}/fork_VERSION
                     -P ${CMAKE_SOURCE_DIR}/versionBuild/version.cmake
)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/version)
ADD_DEPENDENCIES(instrumentall version)

## configure tests
enable_testing ()

# find python
include ( ${PROJECT_SOURCE_DIR}/cmake/python.cmake )

# copy instrumentall.properties file to the bin dir
file (
  COPY "${PROJECT_SOURCE_DIR}/testsuite/resources/test.properties"
  DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  )

# raw tests __without__ test framework
# test that the final binary is executable
if (UNIX)
    add_test (
        NAME instruExec
        COMMAND $<TARGET_FILE:instrumentall>
        )
    add_test (
        NAME instruHelp
        COMMAND $<TARGET_FILE:instrumentall> --help
        )
    add_test (
        NAME instruConf
        COMMAND $<TARGET_FILE:instrumentall> --config-file=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test.properties
        )
else (UNIX)
    add_test (
        NAME instruExec
        COMMAND $<TARGET_FILE:instrumentall>
        )
    add_test (
        NAME instruHelp
        COMMAND $<TARGET_FILE:instrumentall> /help
        )
    add_test (
        NAME instruConf
        COMMAND $<TARGET_FILE:instrumentall> /config-file=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test.properties
        )
endif (UNIX)