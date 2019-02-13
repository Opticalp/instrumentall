/**
 * @file	src/tools/SerialComImpl_Posix.h
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

#ifndef SRC_SERIALCOMIMPL_POSIX_H_
#define SRC_SERIALCOMIMPL_POSIX_H_

#ifdef __unix__

#include <termios.h>
#include <unistd.h>
#include <fcntl.h> // File control definitions
// #include <stdio.h> // standard input / output functions
#include <string.h> // string function definitions (strerror...)
#include <errno.h> // Error number definitions
#include <time.h>   // time calls

#include <string>
#include <vector>

/**
 * SerialComImpl
 *
 * Implementation of the serial communication (POSIX systems)
 */
class SerialComImpl
{
public:
    SerialComImpl();
    virtual ~SerialComImpl();

    /**
     * Open the serial com port
     */
    void open(std::string port);

    /**
     * Close the com port
     */
    void close();

    void setPortSettings(int speed, char parity, int wordSize,
            int stopBits, size_t bufSize);

    /**
     * Check if the port is open and valid
     */
    bool isOpen();

	/**
     * read data from the com port
     *
     * @param buffer allocated char buffer that will be filled
     * with received data
     * @param bufSize buffer size
     * @return size of read data
     */
    size_t read(char* buffer, size_t bufSize);

    /**
     * write data to the com port
     *
     * @param buffer char buffer that is to be sent
     * to the com port
     * @param bufSize buffer size
     * @return number of sent characters
     */
    size_t write(const char* buffer, size_t bufSize);

    /**
     * Retrieve the list of the COM ports
     */
    static void listComPorts(std::vector<std::string>& portList);

protected:
    size_t mBufSize;
    std::string portName;

private:
    int fd; ///< serial com port file descriptor
};

#endif /* __unix__ */
#endif /* SRC_SERIALCOMIMPL_POSIX_H_ */
