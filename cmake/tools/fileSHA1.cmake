## @file     cmake/tools/fileSHA1.cmake
## @date     nov. 2017
## @author   PhRG / opticalp.fr
## @license  MIT

## this script file helps building SHA1 digests for file
## to be used in user management

cmake_minimum_required(VERSION 3.0)

set(scriptFile "prod.py" CACHE FILEPATH "file to be SHA1'd")

file(SHA1 "${scriptFile}" sha1_file)

message("${scriptFile} sha1:")
message("${sha1_file}")

