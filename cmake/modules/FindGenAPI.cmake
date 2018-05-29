# @file     cmake/modules/FindGenAPI.cmake
# @date     may 2017
# @author   PhRG / opticalp.fr
# @license  MIT

# Detecting GenAPI lib installation (emva.org)
# 
# you can set genapi_include_dir and genapi_lib_dir (windows) to help the
# search of the include directory and .lib files
# if genapi_root_dir is set, genapi_include_dir can be defined automatically. 
#
# To use the results of: 
#       find_package ( GenAPI )
# You have to include target link libraries like this:
#       target_link_libraries ( my_executable GenAPI )
#
# ========
# genAPI version check is supported 
# genAPI version EXACT keyword is not supported
#

if (GenAPI_FIND_QUIETLY)
    message ( WARNING "FindGenAPI: QUIET keyword not supported. " )
endif()

if ( GenAPI_FIND_COMPONENTS )
    message ( WARNING "GenAPI: COMPONENTS keyword not supported. " )
endif ( )

if ( GenAPI_FIND_VERSION )
	message ( STATUS "GenAPI requested version is at least: ${GenAPI_FIND_VERSION}" )
	if ( GenAPI_FIND_VERSION_EXACT )
        message ( WARNING "findGenAPI: EXACT keyword is not supported")
	endif ( GenAPI_FIND_VERSION_EXACT )
endif ( GenAPI_FIND_VERSION )

set ( _GenAPI_NOTFOUND_MESSAGE "" )

# safety measure. find_file and find_library write results in CACHE!
unset (_genapi_include_root CACHE)

# variables to be found in cache
set (
  genapi_root_dir ""
  CACHE PATH
  "GenAPI root directory"
  )

set (
  genapi_include_dir ""
  CACHE PATH
  "GenAPI include directory"
  )

if ( genapi_root_dir AND NOT genapi_include_dir )
  string ( CONCAT _genapi_include_dir "${genapi_root_dir}" "/library/CPP/include" )
  set (   genapi_include_dir "${_genapi_include_dir}"
  CACHE PATH
  "GenAPI include directory"
  FORCE
  )
  unset ( _genapi_include_dir )
endif ( genapi_root_dir AND NOT genapi_include_dir )

# verify that some header files are in the include dir
find_path ( 
    _genapi_include_root
    GenICam.h
    ${genapi_include_dir} )

if ( _genapi_include_root STREQUAL "_genapi_include_root-NOTFOUND" )
    set ( _GenAPI_NOTFOUND_MESSAGE 
        "Can not find GenAPI header files. Please check genapi_include_dir variable"
        ) 
else ( _genapi_include_root STREQUAL "_genapi_include_root-NOTFOUND" )
    message ( STATUS "GenICam.h found at ${_genapi_include_root}. " )
endif ( _genapi_include_root STREQUAL "_genapi_include_root-NOTFOUND" )

# check version
find_path ( 
    _genapi_version_h
    _GenICamVersion.h
    ${_genapi_include_root} )

if ( _genapi_version_h STREQUAL "_genapi_version_h-NOTFOUND" )
	message ( WARNING "_GenICamVersion.h not found. " ) 
else ( )
	message ( STATUS "_GenICamVersion.h found. " ) 
    set ( _genapi_version_h "${_genapi_include_root}/_GenICamVersion.h" )
endif ( )

# parsing version
file(STRINGS ${_genapi_version_h} _major REGEX "^#define GENICAM_VERSION_MAJOR ")
file(STRINGS ${_genapi_version_h} _minor REGEX "^#define GENICAM_VERSION_MINOR ")
file(STRINGS ${_genapi_version_h} _patch REGEX "^#define GENICAM_VERSION_SUBMINOR ")
if ( _major AND _minor AND _patch )
    string(REGEX REPLACE "#define GENICAM_VERSION_MAJOR ([0-9]*).*" "\\1" GenAPI_VERSION_MAJOR "${_major}")
    string(REGEX REPLACE "#define GENICAM_VERSION_MINOR ([0-9]*).*" "\\1" GenAPI_VERSION_MINOR "${_minor}")
    string(REGEX REPLACE "#define GENICAM_VERSION_SUBMINOR ([0-9]*).*" "\\1" GenAPI_VERSION_PATCH "${_patch}")

    set ( GenAPI_VERSION "${GenAPI_VERSION_MAJOR}.${GenAPI_VERSION_MINOR}.${GenAPI_VERSION_PATCH}" )
#    message ( STATUS "GenAPI version is ${GenAPI_VERSION}")
else()
    message ( WARNING "Include file ${_genapi_version_h} does not contain expected version information")
endif()

if ( GenAPI_VERSION VERSION_LESS GenAPI_FIND_VERSION )
	set ( _GenAPI_NOTFOUND_MESSAGE "Insufficient GenAPI version: ${GenAPI_VERSION}" )
endif ( )

# Create imported target Poco::${module}
add_library ( GenAPI UNKNOWN IMPORTED )

set_target_properties ( GenAPI PROPERTIES
		  INTERFACE_INCLUDE_DIRECTORIES "${_genapi_include_root}"
		)

# directories containing the .lib files for windows
if (WIN32)
    set (
      genapi_lib_dir ""
      CACHE PATH
      "GenAPI .lib files directory"
      )

	if ( genapi_root_dir AND NOT genapi_lib_dir )
	  string ( CONCAT _genapi_lib_dir "${genapi_root_dir}" "/library/CPP/lib/Win64_x64" )
      if ( EXISTS "${_genapi_lib_dir}" )
        set ( genapi_lib_dir "${_genapi_lib_dir}" CACHE PATH "GenAPI .lib files directory" FORCE)
        message ( STATUS "64-bit GenAPI libs will be used.")
      else ( EXISTS "${_genapi_lib_dir}" )
        string ( CONCAT _genapi_lib_dir "${genapi_root_dir}" "/library/CPP/lib/Win32_i86" )
        if ( EXISTS "${_genapi_lib_dir}" )
          set ( genapi_lib_dir "${_genapi_lib_dir}" CACHE PATH "GenAPI .lib files directory" FORCE)
          message ( STATUS "32-bit GenAPI libs will be used.")
        else ( EXISTS "${_genapi_lib_dir}" )
          message (
            SEND_ERROR 
            "Can not find GenAPI lib files. Please check genapi_lib_dir variable"
            ) 
        endif ( EXISTS "${_genapi_lib_dir}" )
      endif ( EXISTS "${_genapi_lib_dir}" )
	endif ( genapi_root_dir AND NOT genapi_lib_dir )
    
    message ( STATUS "genapi_lib_dir is ${genapi_lib_dir}" )

    # we can not use find_library here, since the lib name is way too complicated...
    file (
      GLOB
      _genapi_lib
      "${genapi_lib_dir}/GenApi*.lib"
      )
    set_target_properties ( GenAPI PROPERTIES
             IMPORTED_LOCATION "${_genapi_lib}"
		)

    file (
      GLOB
      _genapi_libs
      "${genapi_lib_dir}/*.lib"
      )

    message ( STATUS "GenAPI libs are: ${_genapi_libs}" )

    set_target_properties ( GenAPI PROPERTIES
             INTERFACE_LINK_LIBRARIES "${_genapi_libs}"
		)

    unset ( _genapi_libs CACHE )
endif (WIN32)

# cleaning...
unset ( _genapi_include_root CACHE )
unset ( _genapi_version_h CACHE )

if ( _GenAPI_NOTFOUND_MESSAGE )
    set(GenAPI_NOT_FOUND_MESSAGE "${_GenAPI_NOTFOUND_MESSAGE}")
    set(GenAPI_FOUND False)
    if (GenAPI_FIND_REQUIRED)
      message ( SEND_ERROR "$GenAPI_NOT_FOUND_MESSAGE" ) 
    endif (GenAPI_FIND_REQUIRED)
else ( _GenAPI_NOTFOUND_MESSAGE ) 
    set(GenAPI_FOUND True)
endif ( _GenAPI_NOTFOUND_MESSAGE )

