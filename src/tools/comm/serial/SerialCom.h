/**
 * @file	src/tools/SerialCom.h
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

#ifndef SRC_SERIALCOM_H_
#define SRC_SERIALCOM_H_

#ifdef WIN32
#  include "SerialComImpl_Win32.h"
#else /* WIN32 */
#  ifdef __unix__
#    include "SerialComImpl_Posix.h"
#  else
#    include "SerialComImpl_other.h"
#  endif /* __unix__ */
#endif /* WIN32 */

#include <string>
#include <vector>

/**
 * SerialCom
 *
 * Tool to handle serial communication.
 * Handle only serial communication via the standard serial port now.
 *
 * Could handle also UDP com in the future.
 */
class SerialCom: private SerialComImpl
{
public:
    SerialCom(): delimiter('\r') { }
    virtual ~SerialCom() { }

    /**
     * Open the serial com port
     */
    void open(std::string port)
    { SerialComImpl::open(port); }

    /**
     * Close the com port
     */
    void close()
    { SerialComImpl::close(); }

    /**
     * Set port communication settings
     */
    void setPortSettings(int speed=9600, char parity='n', int wordSize=8,
            int stopBits=1, size_t bufSize=1024)
    { SerialComImpl::setPortSettings(speed, parity, wordSize, stopBits, bufSize); }

    /**
     * Check if the port is open and valid
     *
     * @throw Poco::IOException if not valid
     */
    void checkOpen()
    { SerialComImpl::checkOpen(); }

    /**
     * Set the command delimiter
     *
     * to be appended at the end of the commands
     */
    void setDelimiter(char delim)
    { delimiter = delim; }

    /**
     * Read the response
     *
     * @param maxCharCnt maximum number of characters to read.
     * if maxCharCnt is zero, read until timeout
     */
    std::string read(size_t maxCharCnt = 0);

    /**
     * Translate '\n' and '\r' into viewable things
     */
    static std::string niceString(std::string msg);

    /**
     * Write a command
     *
     * add the delimiter at the end
     */
    void write(std::string command);

    std::string deviceName()
        { return portName; }
        
    static void listComPorts(std::vector<std::string>& portList)
        { SerialComImpl::listComPorts(portList); }

private:
    char delimiter;
};

#endif /* SRC_SERIALCOM_H_ */
