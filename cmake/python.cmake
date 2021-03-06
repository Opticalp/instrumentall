## @file     cmake/python.cmake
## @date     nov. 2015
## @author   PhRG / opticalp.fr
## @license  MIT


# If you'd like to specify the installation of Python to use, you should
# modify the following cache variables:
#   PYTHON_LIBRARY             - path to the python library
#   PYTHON_INCLUDE_DIR         - path to where Python.h is found
find_package ( PythonLibs 2.7 )

if ( NOT PYTHONLIBS_FOUND ) 
    message ( WARNING "Python 2.7 not found" )
# require cmake >= 2.8.8 to use PYTHONLIBS_VERSION_STRING
elseif ( NOT PYTHONLIBS_VERSION_STRING MATCHES "^2\\.7\\." )
	message ( WARNING "Python version 2.7 not found. Found: ${PYTHONLIBS_VERSION_STRING}. ")
else ( NOT PYTHONLIBS_FOUND ) 
    add_definitions ( -DHAVE_PYTHON27 )
    message ( STATUS "Python libraries are: ${PYTHON_LIBRARIES}" )
    message ( STATUS "Python include path is: ${PYTHON_INCLUDE_DIRS}" )
    message ( STATUS "Python full version is: ${PYTHONLIBS_VERSION_STRING}" )
    include_directories ( ${PYTHON_INCLUDE_DIRS} )
    target_link_libraries ( instrumentall ${PYTHON_LIBRARIES} )
    
    # building bin tree
    file (MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python)
    file (MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/embed)
    file (MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/scripts)
    file (MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/modScripts)

	# copy the embedded tools
	file (
	  COPY "${PROJECT_SOURCE_DIR}/runtimeEnv/python/embed/instruTools.py"
	  DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/embed/
	  )
    
    # python tests
    file (
        GLOB_RECURSE
        pyTestScripts
        "${PROJECT_SOURCE_DIR}/testsuite/python/*.py"
        )
        
    foreach ( pyTestScript ${pyTestScripts} )
        get_filename_component ( shortPyScriptNoExt "${pyTestScript}" NAME_WE )
        
        if ( UNIX )
            add_test (
                NAME                ${shortPyScriptNoExt}
                COMMAND             $<TARGET_FILE:instrumentall> --execute=${pyTestScript}
                )
        else ( UNIX )
            add_test (
                NAME                ${shortPyScriptNoExt}
                COMMAND             $<TARGET_FILE:instrumentall> /execute=${pyTestScript}
                )
        endif ( UNIX )
        
        get_filename_component ( shortPyScript "${pyTestScript}" NAME )
        message ( STATUS "Python test ${shortPyScript} added. " )
        
    endforeach ( pyTestScript )  
    
endif ( NOT PYTHONLIBS_FOUND ) 
