# @file     CMakeLists.txt
# @date     nov. 2015
# @author   PhRG / opticalp.fr
# @license  MIT

# main cmake file
#  - create a simple project
#  - look for dependencies
#  - link to those dependencies

cmake_minimum_required (VERSION 2.8.8)
project ( Instrumentall )

# set search path for project specific cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

option (no-opencv
    "if specified, don't try to use the OpenCV libraries"    )
option (no-wxwidgets
    "if specified, don't try to import the wxWidgets libraries to build the graphical interface"   )
option (no-genAPI
    "if specified, don't try to use the GenICam API"  ON )
option (loop-gui-script
    "if specified, the GUI script is executed over and over again (until failure or cancellation)" )
option (manage-users
    "if specified, the user permissions are managed" ON) # default to ON for CI tests

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

if (loop-gui-script)
  add_definitions ( -DGUI_SCRIPT_LOOP ) 
endif (loop-gui-script)

if (manage-users)
  include ( ${PROJECT_SOURCE_DIR}/cmake/users.cmake )
endif (manage-users)

if (MSVC)
  add_definitions ( -DNOMINMAX )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif (MSVC)

## configure dependencies

find_package ( Threads REQUIRED )

target_link_libraries(
    instrumentall      
    ${CMAKE_THREAD_LIBS_INIT}
    )    

# find POCO
find_package( Poco 1.3.6 COMPONENTS Foundation Util Net REQUIRED )

message ( STATUS "found Poco version: ${Poco_VERSION}" )
if ( DEFINED Poco_LIBRARIES )
    message ( STATUS "libraries are: ${Poco_LIBRARIES}" )
endif ( DEFINED Poco_LIBRARIES )
    
if ( Poco_VERSION VERSION_GREATER "1.4.0" )
	add_definitions ( -DPOCO_VERSION_H )
endif ( )

target_link_libraries( instrumentall  Poco::Foundation Poco::Util Poco::Net)

# find OpenCV
if ( NOT no-opencv ) 
    find_package( OpenCV REQUIRED )
    message ( STATUS "OpenCV ${OpenCV_VERSION} was found" )
    message ( STATUS "OpenCV installed components are: ${OpenCV_LIB_COMPONENTS}" )
    
    if ( ${OpenCV_VERSION} VERSION_GREATER 3.0 )
        set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
        set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_MINSIZEREL RELEASE)
    else ( ${OpenCV_VERSION} VERSION_GREATER 3.0 )
        message ( SEND_ERROR "OpenCV version less than 3.0. Please update. ")
    endif ( )

    add_definitions ( -DHAVE_OPENCV )
    target_link_libraries( instrumentall ${OpenCV_LIBS} )
endif ( NOT no-opencv ) 

# find wxWidgets
if ( NOT no-wxwidgets )
    # Note that for MinGW users the order of libs is important!
    find_package(wxWidgets REQUIRED COMPONENTS adv xml html xrc core base)

    message ( STATUS "wxWidgets found: ${wxWidgets_LIBRARIES}")
    include(${wxWidgets_USE_FILE})

    message ( STATUS "find_package(wxwidgets) added the definitions: ${wxWidgets_DEFINITIONS}" )
    add_definitions ( -DHAVE_WXWIDGETS )
    target_link_libraries( instrumentall ${wxWidgets_LIBRARIES})
    
    # add some files 
    # include ( ${PROJECT_SOURCE_DIR}/cmake/wxwidgets.cmake )
    
    # or keep it here
    # copy resource files
    file (
      GLOB_RECURSE
      resource_files
      "${PROJECT_SOURCE_DIR}/runtimeEnv/conf/rc/*.xrc"
      "${PROJECT_SOURCE_DIR}/runtimeEnv/conf/rc/*.png"
      "${PROJECT_SOURCE_DIR}/runtimeEnv/conf/rc/*.jpg"
      )
      
    file (
      COPY ${resource_files}
      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/conf/rc/
      FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
      )
endif ( NOT no-wxwidgets )

# find genAPI
if ( NOT no-genAPI ) 
    find_package( GenAPI REQUIRED )
    message ( STATUS "GenAPI ${GenAPI_VERSION} was found" )
    
    add_definitions ( -DHAVE_GENAPI )
    target_link_libraries( instrumentall GenAPI )
endif ( NOT no-genAPI ) 


## generate build info

ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/versionBuild/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version/version.h
                     -D VERSION_FILE=${CMAKE_SOURCE_DIR}/VERSION
                     -D FORK_NAME_FILE=${CMAKE_SOURCE_DIR}/fork_NAME
                     -D FORK_VERSION_FILE=${CMAKE_SOURCE_DIR}/fork_VERSION
                     -D OCV_VERSION=${OpenCV_VERSION}
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
#    add_test (
#        NAME instruExec
#        COMMAND $<TARGET_FILE:instrumentall>
#        )
    add_test (
        NAME instruHelp
        COMMAND $<TARGET_FILE:instrumentall> --help
        )
#    add_test (
#        NAME instruConf
#        COMMAND $<TARGET_FILE:instrumentall> --config-file=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test.properties
#        )
else (UNIX)
#    add_test (
#        NAME instruExec
#        COMMAND $<TARGET_FILE:instrumentall>
#        )
    add_test (
        NAME instruHelp
        COMMAND $<TARGET_FILE:instrumentall> /help
        )
#    add_test (
#        NAME instruConf
#        COMMAND $<TARGET_FILE:instrumentall> /config-file=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test.properties
#        )
endif (UNIX)

# install directives
install ( TARGETS instrumentall DESTINATION . )
install ( DIRECTORY runtimeEnv/
          DESTINATION .
          PATTERN "*.wxg" EXCLUDE
          PATTERN "*/README.txt" EXCLUDE)

