/**
 * @file	src/tools/SerialComImpl_Posix.cpp
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

#ifdef __unix__

#include "SerialComImpl_Posix.h"
#include "Poco/Exception.h"

SerialComImpl::SerialComImpl():
    fd(-1)
{
    // nothing to do
}

SerialComImpl::~SerialComImpl()
{
    try
    {
        close();
    }
    catch (Poco::IOException& )
    {
        // nothing to do
    }
}

void SerialComImpl::open(std::string port)
{
    portName = port;

    fd = ::open(
    //the name of the serial port
    //as a c-string (char *)
    //eg. /dev/ttys0
    port.c_str(),
    //configuration options
    //O_RDWR - we need read
    // and write access
    //O_CTTY - prevent other
    // input (like keyboard)
    // from affecting what we read
    //O_NDELAY - We don’t care if
    // the other side is
    // connected (some devices
    // don’t explicitly connect)
    O_RDWR | O_NOCTTY  | O_NDELAY
    );

    if (fd < 0)
        throw Poco::IOException(
                std::string("SerialCom ") + portName,
                "Unable to open the port");

    // Flush Port
    if (tcflush( fd, TCIFLUSH )<0) // or TCIOFLUSH or TCOFLUSH
        throw Poco::IOException(
                std::string("SerialCom ") + portName,
                "Unable to flush the port");

    // wait for a character to be in the input buffer
    // while read
    fcntl(fd, F_SETFL, 0);
    //fcntl(_fd, F_SETFL, FNDELAY);
}

bool SerialComImpl::isOpen()
{
    return (fd != -1);
}

void SerialComImpl::close()
{
    if (!isOpen())
        return;

    //close the serial port
    if(::close(fd) == -1) {
        throw Poco::IOException(
                std::string("SerialCom ") + portName,
                "Unable to close the file");
    }

    fd = -1;
}

void SerialComImpl::setPortSettings(int speed, char parity, int wordSize,
        int stopBits, size_t bufSize)
{
    mBufSize = bufSize;

    //create the struct
    struct termios options;
    memset(&options, 0, sizeof options);
    // get the current settings of the
    // serial port
    if (tcgetattr(fd, &options) != 0)
    {
        throw Poco::IOException(
                std::string("SerialCom ") + portName,
                "getting current settings failed");
    }

    // set the speed
    speed_t comSpeed;
    switch(speed)
    {
    case 0:
        comSpeed = B0;
        break;
    case 50:
        comSpeed = B50;
        break;
    case 75:
        comSpeed = B75;
        break;
    case 110:
        comSpeed = B110;
        break;
    case 134:
        comSpeed = B134;
        break;
    case 150:
        comSpeed = B150;
        break;
    case 200:
        comSpeed = B200;
        break;
    case 300:
        comSpeed = B300;
        break;
    case 600:
        comSpeed = B600;
        break;
    case 1200:
        comSpeed = B1200;
        break;
    case 1800:
        comSpeed = B1800;
        break;
    case 2400:
        comSpeed = B2400;
        break;
    case 4800:
        comSpeed = B4800;
        break;
    case 9600:
        comSpeed = B9600;
        break;
    case 19200:
        comSpeed = B19200;
        break;
    case 38400:
        comSpeed = B38400;
        break;
    default:
        throw Poco::NotImplementedException(
                "SerialCom::portSettings() unsupported speed value");
    }

    cfsetispeed(&options, comSpeed);
    cfsetospeed(&options, comSpeed);


//    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
//    // disable IGNBRK for mismatched speed tests; otherwise receive break
//    // as \000 chars
//    options.c_iflag &= ~IGNBRK;         // disable break processing
//    options.c_lflag = 0;                // no signaling chars, no echo,
//                                    // no canonical processing
//    options.c_oflag = 0;                // no remapping, no delays
//    options.c_cc[VMIN]  = 0;            // read doesn't block
//    options.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
//
//    options.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
//
//    options.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
//                                    // enable reading
//    options.c_cflag &= ~(PARENB | PARODD);      // shut off parity
//    options.c_cflag |= parity;
//    options.c_cflag &= ~CSTOPB;
//    options.c_cflag &= ~CRTSCTS;


    if (parity == 'n' && wordSize == 8 && stopBits == 1)
    {
        //PARENB is enable parity bit
        //so this disables the parity bit
        options.c_cflag &= ~PARENB;
        //CSTOPB means 2 stop bits
        //otherwise (in this case)
        //only one stop bit
        options.c_cflag &= ~CSTOPB;
        //CSIZE is a mask for all the
        //data size bits, so ending
        //with the negation clears out
        //the current data size setting
        options.c_cflag &= ~CSIZE;
        //CS8 means 8-bits per work
        options.c_cflag |= CS8;
    }
    else
    {
        throw Poco::NotImplementedException(
                "SerialCom::portSettings() unsupported settings");

//        //enable parity with PARENB
//        options.c_cflag |= PARENB;
//        //PARAODD enables odd parity
//        //and with ~PARAODD for even parity
//        options.c_cflag |= PARODD;
//        //Only one stop bit
//        options.c_cflag &= ~CSTOPB;
//        //clear out the current word size
//        options.c_cflag &= ~CSIZE;
//        //we only have 7-bit words here
//        //because one bit is taken up
//        //by the parity bit
//        options.c_cflag |= CS7;
    }

    options.c_cflag &= ~CRTSCTS;       // no flow control
//    options.c_cflag |= CRTSCTS;  // flow control

    //CLOCAL means don’t allow
    //control of the port to be changed
    //CREAD says to enable the receiver
    options.c_cflag |= (CLOCAL | CREAD);

    //Set the timeouts
    //VMIN is the minimum amount
    //of characters to read.
    options.c_cc[VMIN] = 1;

    //The amount of time to wait
    //for the amount of data
    //specified by VMIN in tenths
    //of a second.
    options.c_cc[VTIME] = 10;

    cfmakeraw(&options); // Make raw (useful?)

//    // Flush Port, then applies attributes
//    tcflush( _fd, TCIFLUSH ); // or TCIOFLUSH or TCOFLUSH

    // TCSANOW - apply the changes now
    // TCSADRAIN - wait until every
    // thing has been transmitted
    // TCSAFLUSH - flush buffers
    // and apply changes
    if(tcsetattr(fd, TCSANOW, &options)!= 0) {
        throw Poco::IOException(
                std::string("SerialCom ") + portName,
                "settings changing failed");
    }
}

size_t SerialComImpl::read(char* buffer, size_t bufSize)
{
    size_t cumulator = 0;
    long int read1 = 1;

    while (read1 && cumulator < bufSize)
    {
        // usleep(100000); // 100ms

        read1 = ::read(fd, buffer+cumulator, 1);

        if (read1 < 0)
            throw Poco::IOException(::strerror(errno));

        cumulator++;
    }

    return static_cast<size_t>(cumulator);
}

size_t SerialComImpl::write(const char* buffer, size_t bufSize)
{
    return ::write(fd, buffer, bufSize);
}

void SerialComImpl::listComPorts(std::vector<std::string>& portList)
{
    // see https://stackoverflow.com/q/2530096
}

#endif /* __unix__ */
