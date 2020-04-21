/**
 * @file	src/tools/comm/Decorated.h
 * @date	Apr. 2018
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2018 Ph. Renaud-Goud / Opticalp

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef SRC_TOOLS_DECORATED_H_
#define SRC_TOOLS_DECORATED_H_

#include <string>

/**
 * Replace \0 by explicit '\0'
 * Replace \r by explicit '\r'
 * Replace \n by explicit '\n'
 */
std::string decoratedCommand(std::string command);

/**
 * Replace \0 by explicit '\0'
 * Replace \r by explicit '\r'
 * Replace \n by explicit '\n'
 *
 * @param command string to be parsed
 * @param length required length. If length == command.size() + 1, a null terminator is appended
 */
std::string decoratedCommand(std::string command, size_t length);

/**
 * Replace \0 by explicit '\0'
 * Replace \r by explicit '\r'
 * Replace \n by explicit '\n'
 *
 * Keep a cariage return
 */
std::string decoratedCommandKeep(std::string command);

/**
* Replace \0 by explicit '\0'
* Replace \r by explicit '\r'
* Replace \n by explicit '\n'
*
* Keep a cariage return
*/
std::string decoratedCommandKeep(std::string command, size_t length);

/**
 * Replace '\01' by explicit "01" etc
 */
std::string decoratedHexCommand(std::string command);

/**
 * Replace "01" by '\01' etc
 *
 * remove "0x" prefix if present
 */
std::string rawByteFromString(std::string hexStr);

#endif /* SRC_TOOLS_DECORATED_H_ */
