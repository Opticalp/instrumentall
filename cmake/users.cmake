## @file     cmake/users.cmake
## @date     nov. 2017
## @author   PhRG / opticalp.fr
## @license  MIT

## this config file is loaded if the user permissions are managed

message (STATUS "Configuring users")

add_definitions ( -DMANAGE_USERS )

set (USER_ADMIN_NAME "admin")
set (USER_ADMIN_PWD  "1234abcdABCD" CACHE STRING "admin user account password")
set (USER_ADMIN_UID  "1")
set (USER_ADMIN_DESCR  "User with administrative rights")

set (USER_ANONYMOUS_NAME "anonymous")
set (USER_ANONYMOUS_UID  "0")
set (USER_ANONYMOUS_DESCR  "Anonymous user (not logged in)")

set (USER_NAME "user1" CACHE STRING "example user account name")
set (USER_PWD  "john is not jack" CACHE STRING "example user account password")
set (USER_UID  "2" CACHE STRING "example user ID (integer)")
set (USER_DESCR  "some user" CACHE STRING "example user account description")

set (USER_PWD_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/access/.passwd")

string (SHA1 USER_ADMIN_DIGEST ${USER_ADMIN_PWD})
string (SHA1 USER_DIGEST ${USER_PWD})

file (WRITE ${USER_PWD_FILE} "${USER_ANONYMOUS_NAME}:${USER_ANONYMOUS_UID}:${USER_ANONYMOUS_DESCR}:\n")
file (APPEND ${USER_PWD_FILE} "${USER_ADMIN_NAME}:${USER_ADMIN_UID}:${USER_ADMIN_DESCR}:${USER_ADMIN_DIGEST}\n")
file (APPEND ${USER_PWD_FILE} "${USER_NAME}:${USER_UID}:${USER_DESCR}:${USER_DIGEST}\n")

# install directives
install ( FILES ${USER_PWD_FILE} DESTINATION ./access )