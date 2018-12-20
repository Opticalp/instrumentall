/**
 * @file	src/tools/SerialComImpl_other.h
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

#ifndef SRC_SERIALCOMIMPL_OTHER_H_
#define SRC_SERIALCOMIMPL_OTHER_H_

#ifndef __unix__
#ifndef WIN32

#include "Poco/Exception.h"

#include <string>

/**
 * SerialComImpl
 *
 * (non-)Implementation of the serial communication (unknown system)
 */
class SerialComImpl
{
public:
    SerialComImpl() { }
    virtual ~SerialComImpl() { }

    /**
     * Open the serial com port
     */
    void open(std::string comPort)
    {
        throw Poco::NotImplementedException(
            "SerialCom::open() is not implemented "
            "for this platform");
    }

    /**
     * Close the com port
     *
     * @throw Poco::IOException if any problem occurs
     */
    void close()
    {
        throw Poco::NotImplementedException(
            "SerialCom::close() is not implemented "
            "for this platform");
    }

    void setPortSettings(int speed, char parity, int wordSize,
            int stopBits, size_t bufSize)
    {
        throw Poco::NotImplementedException(
            "SerialCom::setPortSettings() is not implemented "
            "for this platform");
    }

    /**
     * Check if the port is open and valid
     *
     * @throw Poco::IOException if not valid
     */
    void checkOpen()
    {
        throw Poco::NotImplementedException(
            "SerialCom::checkOpen() is not implemented "
            "for this platform");
    }

    /**
     * read data from the com port
     *
     * @param buffer allocated char buffer that will be filled
     * with received data
     * @param bufSize buffer size
     * @return size of read data
     */
    size_t read(char* buffer, size_t bufSize)
    {
        throw Poco::NotImplementedException(
            "SerialCom::read() is not implemented "
            "for this platform");
    }

    /**
     * write data to the com port
     *
     * @param buffer char buffer that is to be sent
     * to the com port
     * @param bufSize buffer size
     * @return number of sent characters
     */
    size_t write(const char* buffer, size_t bufSize)
    {
        throw Poco::NotImplementedException(
            "SerialCom::write() is not implemented "
            "for this platform");
    }

protected:
    size_t mBufSize;
    std::string portName;

};

#endif /*   WIN32  */
#endif /* __unix__ */
#endif /* SRC_SERIALCOMIMPL_OTHER_H_ */
