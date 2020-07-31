# @file     cmake/modules/FindPoco.cmake
# @date     nov. 2015
# @author   PhRG / opticalp.fr
# @license  MIT

# Detecting POCO lib installation (pocoproject.org)
# 
# for older versions of Poco, PocoConfig.cmake is not available. 
# Poco has to be found "manually", then...
# 
# you can set poco_include_dir and poco_lib_dir (windows) to help the
# search of the include directory and .lib files
#
# To use the results of: 
#       find_package ( Poco COMPONENTS Foundation )
# You have to include target link libraries like this:
#       target_link_libraries ( my_executable Poco::Foundation )
#
# ========
# poco components is supported
# poco version check is supported 
# poco version EXACT keyword is not supported
#


set(_Poco_FIND_PARTS_REQUIRED)
if (Poco_FIND_REQUIRED)
    set(_Poco_FIND_PARTS_REQUIRED REQUIRED)
endif()
set(_Poco_FIND_PARTS_QUIET)
if (Poco_FIND_QUIETLY)
    set(_Poco_FIND_PARTS_QUIET QUIET)
endif()

# avoiding recursion
set ( __store ${CMAKE_MODULE_PATH} )
unset ( CMAKE_MODULE_PATH ) 

find_package ( Poco 
    COMPONENTS
    ${Poco_FIND_COMPONENTS}
    QUIET
)

set ( CMAKE_MODULE_PATH ${__store} )
unset ( __store )

if ( Poco_FOUND )
    message ( STATUS "Poco found automatically" )
	return ( )
endif ( )

# if not found, doing it "manually"

if ( NOT Poco_FIND_COMPONENTS )
    message ( STATUS "Poco: no component specified. Default is: Foundation" )
    set ( _poco_components "Foundation")
elseif ( )
    set ( _poco_components ${Poco_FIND_COMPONENTS} )
endif ( )

if ( Poco_FIND_VERSION )
	message ( STATUS "Poco requested version is at least: ${Poco_FIND_VERSION}" )
	if ( Poco_FIND_VERSION_EXACT )
        message ( WARNING "findPoco: EXACT keyword is not supported")
	endif ( Poco_FIND_VERSION_EXACT )
endif ( Poco_FIND_VERSION )

set ( _Poco_NOTFOUND_MESSAGE "" )

# looking for general paths

# safety measure. find_file and find_library write results in CACHE!
unset (_poco_include_root CACHE)

# variables to be found in cache
set (
  poco_include_dir ""
  CACHE PATH
  "Poco lib header root directory"
  )

# directories containing the .lib files for windows
if (WIN32)
    set (
      poco_lib_dir ""
      CACHE PATH
      "Poco .lib files directory"
      )
endif (WIN32)

# verify that some header files are in it
find_path ( 
    _poco_include_root
    Poco/Poco.h
    ${poco_include_dir} )

if ( _poco_include_root STREQUAL "_poco_include_root-NOTFOUND" )
    message (
        SEND_ERROR 
        "Can not find poco header files. Please check poco_include_dir variable"
        ) 
else ( _poco_include_root STREQUAL "_poco_include_root-NOTFOUND" )
    message ( STATUS "Poco/Poco.h found at ${_poco_include_root}" )
#    if ( ( NOT poco_include_dir ) OR ( check_poco_include_root STREQUAL poco_include_dir ) )
#        include_directories (${check_poco_include_root})
#        set ( Poco_INCLUDE_DIRS "${check_poco_include_root}" )
#    endif ( )
endif ( _poco_include_root STREQUAL "_poco_include_root-NOTFOUND" )

# check version
find_path ( 
    _poco_version_h
    Poco/Version.h
    ${_poco_include_root} )

if ( _poco_version_h STREQUAL "_poco_version_h-NOTFOUND" )
	message ( STATUS "Version.h not found. Poco version < 1.4.0" ) 
    set ( _poco_version_h "${_poco_include_root}/Poco/Foundation.h" )
else ( )
	message ( STATUS "Version.h found. Poco version >= 1.4.0" ) 
    set ( _poco_version_h "${_poco_include_root}/Poco/Version.h" )
endif ( )

# parsing version
file(STRINGS ${_poco_version_h} _contents REGEX "^#define POCO_VERSION 0x")
if(_contents)
    message ( STATUS "POCO_VERSION (hex) define macro is: ${_contents}")
    string(REGEX REPLACE ".*#define POCO_VERSION 0x([0-9A-F][0-9A-F])[0-9A-F]*$" "\\1" Poco_VERSION_MAJOR "${_contents}")
    math(EXPR Poco_VERSION_MAJOR "0x${Poco_VERSION_MAJOR}")
    string(REGEX REPLACE ".*#define POCO_VERSION 0x[0-9A-F][0-9A-F]([0-9A-F][0-9A-F])[0-9A-F]*$" "\\1" Poco_VERSION_MINOR "${_contents}")
    math(EXPR Poco_VERSION_MINOR "0x${Poco_VERSION_MINOR}")
    string(REGEX REPLACE ".*#define POCO_VERSION 0x[0-9A-F][0-9A-F][0-9A-F][0-9A-F]([0-9A-F][0-9A-F])[0-9A-F]*$" "\\1" Poco_VERSION_PATCH "${_contents}")
    math(EXPR Poco_VERSION_PATCH "0x${Poco_VERSION_PATCH}")
    set(Poco_VERSION "${Poco_VERSION_MAJOR}.${Poco_VERSION_MINOR}.${Poco_VERSION_PATCH}" )
#    message ( STATUS "poco version is ${Poco_VERSION}")
else()
    set ( _Poco_NOTFOUND_MESSAGE "Include file ${_poco_version_h} does not contain expected version information")
endif()

if ( Poco_VERSION VERSION_LESS Poco_FIND_VERSION )
	set ( _Poco_NOTFOUND_MESSAGE "Insufficient poco version" )
    message ( STATUS "Poco version (${Poco_VERSION}) too low..." )
endif ( )

foreach ( module ${Poco_FIND_COMPONENTS} )
    message ( STATUS "Looking for poco component: ${module}" )

	unset (_poco_lib CACHE)
	find_library (_poco_lib "Poco${module}" ${poco_lib_dir})
	if (_poco_lib STREQUAL "_poco_lib-NOTFOUND")
        if ( Poco_FIND_REQUIRED_${module} )
            set ( _Poco_NOTFOUND_MESSAGE "${_Poco_NOTFOUND_MESSAGE}Failed to find Poco component \"${module}\"\n" )
        elseif ( NOT Poco_FIND_QUIETLY )
            message ( WARNING "Failed to find Poco component \"${module}\"" )
        endif ( )
	else (_poco_lib STREQUAL "_poco_lib-NOTFOUND")
		message ( 
		    STATUS
		    "Poco${module} lib found: ${_poco_lib}"
		    )

		# Create imported target Poco::${module}
		add_library ( Poco::${module} UNKNOWN IMPORTED )
		
		
		# check include tree
		unset ( _poco_include_module CACHE )
		find_path ( 
            _poco_include_module
            ${module}.h
            "${_poco_include_root}/Poco" "${_poco_include_root}/Poco/${module}" )
		
	
	    message( STATUS "Poco::${module} headers are at: ${_poco_include_module}" )
	
		set_target_properties ( Poco::${module} PROPERTIES
		  INTERFACE_INCLUDE_DIRECTORIES "${_poco_include_root}"
		  IMPORTED_LOCATION "${_poco_lib}"
		)
	
		# NOTE: anything else to do?

	    set ( Poco${module}_FOUND True )

	endif (_poco_lib STREQUAL "_poco_lib-NOTFOUND")
endforeach ( module ${Poco_FIND_COMPONENTS} )

# cleaning...
unset ( _poco_include_module CACHE )
unset ( _poco_lib CACHE )
unset ( _poco_include_root CACHE )
unset ( _poco_version_h CACHE )

if ( _Poco_NOTFOUND_MESSAGE )
    set(Poco_NOT_FOUND_MESSAGE "${_Poco_NOTFOUND_MESSAGE}")
    set(Poco_FOUND False)
else ( _Poco_NOTFOUND_MESSAGE ) 
    set(Poco_FOUND True)
endif ( _Poco_NOTFOUND_MESSAGE )

