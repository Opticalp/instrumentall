# @file     version/version.cmake
# @date     jan. 2016
# @author   PhRG / opticalp.fr
# @license  MIT

##
# Generate build version information 
#
#  - VERSION
#  - BUILD_DATE
#  - GIT_BRANCH
# are set (as string)
##


message ( STATUS "Version header generation" )
message ( STATUS "VERSION file is: ${VERSION_FILE}" )
message ( STATUS "fork_NAME file is: ${FORK_NAME_FILE}" )
message ( STATUS "fork_VERSION file is: ${FORK_VERSION_FILE}" )

# get VERSION
file ( READ ${VERSION_FILE} VERSION_FROM_FILE LIMIT 16 )
string ( STRIP ${VERSION_FROM_FILE} VERSION_FROM_FILE )

# check if is a fork
file ( READ ${FORK_NAME_FILE} FORK_NAME LIMIT 255 )

if ( FORK_NAME STREQUAL "" )
    message (STATUS "This branch is not a fork")
    set ( IS_FORK false )

else ()
    string ( STRIP ${FORK_NAME} FORK_NAME )
    set ( IS_FORK true )
    
    # copy VERSION_FROM_FILE to ROOT_VERSION
    set ( ROOT_VERSION ${VERSION_FROM_FILE} )

    set ( VERSION_FILE ${FORK_VERSION_FILE} )
    
    file ( READ ${VERSION_FILE} VERSION_FROM_FILE LIMIT 16 )
    string ( STRIP ${VERSION_FROM_FILE} VERSION_FROM_FILE )    
    
endif ()

get_filename_component(OutDir ${DST} DIRECTORY)

# append git commit count since last VERSION_FILE modification
FIND_PACKAGE(Git)
if(GIT_FOUND)
  get_filename_component(WD ${VERSION_FILE} DIRECTORY)
  EXECUTE_PROCESS(
    COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%h
    WORKING_DIRECTORY ${WD}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  ) 
  message ( STATUS "Repository git commit hash: ${GIT_HASH}" )

  EXECUTE_PROCESS(
    COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=format:%h -- ${VERSION_FILE}
    WORKING_DIRECTORY ${WD}
    OUTPUT_VARIABLE VERSION_LAST_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  ) 
  message ( STATUS "${VERSION_FILE} git commit hash: ${VERSION_LAST_COMMIT_HASH}" )

  EXECUTE_PROCESS(
    COMMAND ${GIT_EXECUTABLE} log ${VERSION_LAST_COMMIT_HASH}.. --pretty=format:%h
    WORKING_DIRECTORY ${WD}
    OUTPUT_VARIABLE VERSION_SINCE_LAST_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if ( NOT ${VERSION_SINCE_LAST_COMMIT} STREQUAL "")
      # create list
      string (REPLACE "\n" ";" COUNT_ME ${VERSION_SINCE_LAST_COMMIT})
      # count list items
      list(LENGTH COUNT_ME len)
      message ( STATUS "${len} commits since last ${VERSION_FILE} modification" )
  else ( )
      set ( len 0 )
  endif ( )

  string ( CONCAT VERSION_MAIN ${VERSION_FROM_FILE} "." ${len} )
  
  EXECUTE_PROCESS(
    COMMAND ${GIT_EXECUTABLE} branch
    WORKING_DIRECTORY ${WD}
    OUTPUT_VARIABLE BRANCHES
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set ( GIT_BRANCH )

  # create list (in place)
  string (REPLACE "\n" ";" BRANCHES ${BRANCHES})
  foreach ( branch IN LISTS BRANCHES )
    string ( REGEX MATCHALL "^\\* .*$" matchBRANCH ${branch} )
    if (NOT ${matchBRANCH} STREQUAL "")
      string ( SUBSTRING ${branch} 2 -1 GIT_BRANCH )
    endif ( )
  endforeach ( branch )
    
  # get remote URL
  if ((GIT_VERSION_STRING VERSION_GREATER 2.7) OR (GIT_VERSION_STRING VERSION_EQUAL 2.7))
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} remote get-url origin
      WORKING_DIRECTORY ${WD}
      OUTPUT_VARIABLE GIT_URL
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string ( CONCAT GIT_BRANCH ${GIT_BRANCH} " from " ${GIT_URL} )
  endif ( )
  
  message ( STATUS "git branch: ${GIT_BRANCH}" )
else (GIT_FOUND)
  set (VERSION_MAIN ${VERSION_FROM_FILE})
  set ( GIT_BRANCH "local unsync copy" )
endif (GIT_FOUND)

if (EXISTS "${OutDir}/last_version")
  file ( READ ${OutDir}/last_version LAST_VERSION )
  if ( NOT "${LAST_VERSION}" STREQUAL "${VERSION_MAIN}" )
    message ( STATUS "Main version changed. Reinitializing build counter. ")
    file ( REMOVE ${OutDir}/local_build_count )
  endif ()
else (EXISTS "${OutDir}/last_version")
  file ( REMOVE ${OutDir}/local_build_count )
endif (EXISTS "${OutDir}/last_version")

file ( WRITE ${OutDir}/last_version "${VERSION_MAIN}")

# get build count
if ( EXISTS "${OutDir}/local_build_count" )
  file ( READ ${OutDir}/local_build_count PREVIOUS_BUILD_CNT LIMIT 16 )
  math ( EXPR newCnt " ${PREVIOUS_BUILD_CNT} + 1 " )
# create with inc = 0
else ()
  message ( STATUS "local_build_count: file not found. Creation. " )
  set ( newCnt "0" )
endif()

file ( WRITE ${OutDir}/local_build_count "${newCnt}" )

# append count
string ( CONCAT VERSION ${VERSION_MAIN} "." ${newCnt} )

message ( STATUS "Version string is: ${VERSION}" )

string ( TIMESTAMP BUILD_DATE UTC)
message ( STATUS "Build date set to: ${BUILD_DATE}" )

CONFIGURE_FILE(${SRC} ${DST} @ONLY)
