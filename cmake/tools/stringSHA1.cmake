## @file     cmake/tools/stringSHA1.cmake
## @date     nov. 2017
## @author   PhRG / opticalp.fr
## @license  MIT

## this script file helps building SHA1 digests for file
## to be used in user management

cmake_minimum_required(VERSION 3.0)

set(password "1234" CACHE STRING "String to be SHA1'd")

string(SHA1 sha1_string "${password}")

message("string sha1:")
message("${sha1_string}")

