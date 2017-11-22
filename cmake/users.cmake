## @file     cmake/users.cmake
## @date     nov. 2017
## @author   PhRG / opticalp.fr
## @license  MIT

## this config file is loaded if the user permissions are managed

add_definitions ( -DMANAGE_USERS )

message (STATUS "Configuring users")

set (USER_ADMIN_NAME "admin")
set (USER_ADMIN_PWD  "1234abcdABCD" CACHE STRING "admin user account password")
set (USER_ADMIN_UID  "1")
set (USER_ADMIN_DESCR  "User with administrative rights")

set (USER_ANONYMOUS_UID  "0")

set (USER_NAME "user1" CACHE STRING "example user account name")
set (USER_PWD  "john is not jack" CACHE STRING "example user account password")
set (USER_UID  "2" CACHE STRING "example user ID (integer)")
set (USER_DESCR  "some user" CACHE STRING "example user account description")

set (USER_PWD_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/access/.passwd")

string (SHA1 USER_ADMIN_DIGEST ${USER_ADMIN_PWD})
string (SHA1 USER_DIGEST ${USER_PWD})

file (WRITE ${USER_PWD_FILE} "${USER_ADMIN_NAME}:${USER_ADMIN_UID}:${USER_ADMIN_DESCR}:${USER_ADMIN_DIGEST}\n")
file (APPEND ${USER_PWD_FILE} "${USER_NAME}:${USER_UID}:${USER_DESCR}:${USER_DIGEST}\n")

message (STATUS "Configuring user permissions") 

set (USER_ANONYMOUS_PERM_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/access/.UID-${USER_ANONYMOUS_UID}")

# permissions without digest
file (WRITE ${USER_ANONYMOUS_PERM_FILE} "folder:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/scripts:\n") 
file (APPEND ${USER_ANONYMOUS_PERM_FILE} "folder:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/embed:\n")
file (APPEND ${USER_ANONYMOUS_PERM_FILE} "folder:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python/modScripts:\n")
file (APPEND ${USER_ANONYMOUS_PERM_FILE} "folder:${PROJECT_SOURCE_DIR}/testsuite/python:\n")
## could have used
#file (APPEND ${USER_ANONYMOUS_PERM_FILE} "folder:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/python:\n")

## permissions with digest
#file (APPEND ${USER_ANONYMOUS_PERM_FILE} "script:dataflow.py:563e69712cdd95e12749c30434bb8b7755a16523\n")
#file (APPEND ${USER_ANONYMOUS_PERM_FILE} "script:prod.py:563e69712cdd95e12749c30434bb8b7755a16523\n")

# test:
file (APPEND ${USER_ANONYMOUS_PERM_FILE} "script:generalInfo.py:d3d54374bc3cb0ffeb617a853c59173173fae49a\n")

# install directives
install ( FILES ${USER_PWD_FILE} DESTINATION ./access )
install ( FILES ${USER_ANONYMOUS_PERM_FILE} DESTINATION ./access )
install ( CODE "message(\"User permissions. For enough safety, you should consider write-protecting the folders: access/ and python/embed/\")")
