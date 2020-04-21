/**
 * @file	src/tools/SerialCom.cpp
 * @date	Mar 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#include "SerialCom.h"

#include "Poco/Exception.h"
#include "Poco/StringTokenizer.h"

using Poco::StringTokenizer;

#define BUFFER_SIZE 1024
#define COUNT_LIMIT 4

void SerialCom::checkOpen()
{
    if (!isOpen())
        throw Poco::IOException(
                std::string("SerialCom ") + portName,
                "Invalid serial com port file handle");
}

void SerialCom::noDelimiter()
{
	hasDelimiter = false;
}

void SerialCom::setDelimiter(char delim)
{
	delimiter = delim; 
	hasDelimiter = true;
}

std::string SerialCom::read(size_t maxCharCnt)
{
	checkOpen();

    // if (maxCharCnt > BUFFER_SIZE)
    // undefined behavior

    std::string stack;

    char buffer[BUFFER_SIZE];
    size_t ret;

    if (maxCharCnt)
    {
        ret = SerialComImpl::read(buffer, maxCharCnt);
        stack.insert(stack.size(), buffer, ret);
    }
    else
    {
        ret = SerialComImpl::read(buffer, BUFFER_SIZE);
        stack.insert(stack.size(), buffer, ret);

        size_t count = 1;

        // if the buffer was full, read again.
        while (ret == BUFFER_SIZE)
        {
            ret = SerialComImpl::read(buffer, BUFFER_SIZE);
            stack.insert(stack.size(), buffer, ret);
            if (++count >= COUNT_LIMIT)
                throw Poco::IOException("SerialCom::readStack",
                        "recursive read limit");
        }
    }

    return stack;
}

void SerialCom::write(std::string command)
{
	checkOpen();

	if (hasDelimiter)
		command += delimiter;

    if (command.size() <= mBufSize)
    {
        if (SerialComImpl::write(command.c_str(), command.size()) != command.size())
            throw Poco::IOException(
                    std::string("SerialCom ") + portName,
                    "unable to write the full data to the port");
    }
    else
    {
        throw Poco::NotImplementedException("SerialCom::write(string)",
                "String size bigger than the buffer size. "
                "We do not know how to handle that now. ");
    }
}

std::string SerialCom::sendQuery(std::string query, size_t maxCharCnt)
{
	write(query);

	return read(maxCharCnt);
}

#include "tools/comm/Decorated.h"

std::string SerialCom::niceString(std::string msg)
{
    return decoratedCommandKeep(msg);
}
