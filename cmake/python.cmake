## @file     cmake/python.cmake
## @date     nov. 2015
## @author   PhRG / opticalp.fr
## @license  MIT


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

    # building bin tree
    file (MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python)
    file (MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/embed)
    file (MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/scripts)

endif ( NOT PYTHONLIBS_FOUND )
