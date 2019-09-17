/**
 * @file	src/tools/comm/Decorated.cpp
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

#include "Decorated.h"
#include <stdexcept>

std::string decoratedCommand(std::string command)
{
    size_t place;

	place = command.find('\0');
	while (place != std::string::npos)
	{
		command.replace(place, 1, "\\0");
		place = command.find('\0');
	}
	
	place = command.find('\r');
    while (place != std::string::npos)
    {
        command.replace(place,1,"\\r");
        place = command.find('\r');
    }

    place = command.find('\n');
    while (place != std::string::npos)
    {
        command.replace(place,1,"\\n");
        place = command.find('\n');
    }

    return command;
}

std::string decoratedCommand(std::string command, size_t length)
{
	if (length <= command.size())
		return decoratedCommand(command.substr(0, length));
	else if (length == command.size() + 1)
		return decoratedCommand(command) + "\\0";
	else
		throw std::out_of_range("string overflow");
}

std::string decoratedCommandKeep(std::string command)
{
    size_t place;

	place = command.find('\0');
	while (place != std::string::npos)
	{
		command.replace(place, 1, "\\0\0");
		place = command.find('\0', place + 3);
	}

	place = command.find('\n');
    while (place != std::string::npos)
    {
        command.replace(place,1,"\\n\n");
        place = command.find('\n', place + 3);
    }

    place = command.find('\r');
    while (place != std::string::npos)
    {
        command.replace(place,1,"\\r\n");
        place = command.find('\r', place + 3);
    }

    return command;
}

std::string decoratedCommandKeep(std::string command, size_t length)
{
	if (length <= command.size())
		return decoratedCommandKeep(command.substr(0, length));
	else if (length == command.size() + 1)
		return decoratedCommandKeep(command) + "\\0\n";
	else
		throw std::out_of_range("string overflow");
}