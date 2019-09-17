/**
 * @file	src/tools/SerialComImpl_Win32.cpp
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

#ifdef WIN32
#include "SerialComImpl_Win32.h"

#include "Poco/NumberFormatter.h"

#include <tchar.h>

SerialComImpl::SerialComImpl():
    fileHandle(INVALID_HANDLE_VALUE)
{
    // nothing else to do
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
    std::wstring ws_portName;
	port = "\\\\.\\" + port;
    ws_portName.assign(port.begin(), port.end());

    fileHandle = CreateFileW(
        //the name of the port (as a string)
        //eg. COM1
        ws_portName.c_str(),
        //must have read AND write access to port
        GENERIC_READ | GENERIC_WRITE,
        //sharing mode
        //ports CANNOT be shared (hence the 0)
        0,
        //security attributes
        //0 here means that this file handle
        //cannot be inherited
        0,
        //we cannot create serial ports
        OPEN_EXISTING,
        //Overlapped/Non-Overlapped Mode.
        //To use overlapped communication
        //replace 0 with
        //FFILE_FLAG_OVERLAPPED
        0,
        //HANDLE of a template file
        //which will supply attributes and
        //permissions. Not used with port access.
        0);

    // throw exception if the opening failed.
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        std::string errorMsg;
        switch (GetLastError())
        {
        case ERROR_ACCESS_DENIED:
            errorMsg = "acces denied. port probably already in use";
            break;
        case ERROR_FILE_NOT_FOUND:
            errorMsg = "not found. no such port on this system";
            break;
        default:
            errorMsg = "unknown error";
        }

        throw Poco::IOException(
                std::string("SerialCom ") + port,
                errorMsg);
    }

	// if success
	portName = port;
}

bool SerialComImpl::isOpen()
{
    return !(fileHandle == INVALID_HANDLE_VALUE);
}

void SerialComImpl::close()
{
    if (!isOpen())
        return;

    //Close the fileHandle, thus releasing the device.
    if(!CloseHandle(fileHandle))
    {
        throw Poco::IOException(
                std::string("SerialCom ") + portName,
                "Unable to close the file");
    }

    fileHandle = INVALID_HANDLE_VALUE;
	portName = "";
}

void SerialComImpl::setPortSettings(int speed, char parity, int wordSize,
        int stopBits, size_t bufSize)
{
    //checkOpen();

    mBufSize = bufSize;

    DCB dcb = {0};
    dcb.DCBlength=sizeof(dcb);

    if (!GetCommState(fileHandle,&dcb))
    {
        throw Poco::IOException("SerialCom::portSettings()",
                "unable to get the current the DCB");
    }

    if (wordSize <= 4 || wordSize > 8)
    	throw Poco::RangeException("setPortSettings",
    			"Invalid data bits count");

    dcb.ByteSize = wordSize; // data bits count

    switch (speed)
    {
	case    110:
		dcb.BaudRate = CBR_110;
		break;
	case    300:
		dcb.BaudRate = CBR_300;
		break;
	case    600:
		dcb.BaudRate = CBR_600;
		break;
	case   1200:
		dcb.BaudRate = CBR_1200;
		break;
	case   2400:
		dcb.BaudRate = CBR_2400;
		break;
	case   4800:
		dcb.BaudRate = CBR_4800;
		break;
	case   9600:
		dcb.BaudRate = CBR_9600;
		break;
	case  14400:
		dcb.BaudRate = CBR_14400;
		break;
	case  19200:
		dcb.BaudRate = CBR_19200;
		break;
	case  38400:
		dcb.BaudRate = CBR_38400;
		break;
	case  57600:
		dcb.BaudRate = CBR_57600;
		break;
	case 115200:
		dcb.BaudRate = CBR_115200;
		break;
	default:
		throw Poco::InvalidArgumentException("setPortSettings",
				"comm speed not supported");
    }

    if (parity == 'n')
    {
        dcb.Parity = NOPARITY; //no parity

        if ( stopBits == 1 )
        dcb.StopBits = ONESTOPBIT;
        else if (stopBits == 2 )
        dcb.StopBits = TWOSTOPBITS;
        else
        dcb.StopBits = ONE5STOPBITS;
    }
    else
    {
        throw Poco::NotImplementedException(
                "SerialCom::portSettings() unsupported settings");
    }

    if (!SetCommState(fileHandle,&dcb))
    {
        throw Poco::IOException("SerialCom::portSettings()",
                "unable to set the DCB");
    }

    // set the buffers to be size 1024
    //of fileHandle
    //Also returns a boolean indicating
    //success or failure
    if(!SetupComm(fileHandle,
            static_cast<DWORD>(mBufSize),    // in queue
            static_cast<DWORD>(mBufSize) ) ) // out queue
    {
        throw Poco::IOException("SerialCom::portSettings()",
                        "IO buffer creation error");
    }

   // configuring timeouts ***Mandatory***
   COMMTIMEOUTS cmt;

    // the maximum amount of time
    // allowed to pass between
    // the arrival of two bytes on
    // the read line (in ms)
    cmt.ReadIntervalTimeout = (1000/speed+1);
    // total time needed for a read operation
    // (num bytes to read) * (timeoutMultiplier)
    // in ms
    cmt.ReadTotalTimeoutMultiplier = (1000/speed+1)*4;
    // This value is added to
    // the previous one to generate
    // the timeout value
    // for a single read operation (in ms)
    cmt.ReadTotalTimeoutConstant = 500;
    // the next two values are the same
    // as their read counterparts, only
    // applying to write operations
    cmt.WriteTotalTimeoutConstant = 500;
    cmt.WriteTotalTimeoutMultiplier = (1000/speed+1)*4;
    // set the timeouts of fileHandle to be
    // what is contained in cmt
    // returns boolean success or failure
    if(!SetCommTimeouts(fileHandle, &cmt)) {
        throw Poco::IOException("SerialCom::portSettings()",
                        "can not set the timeout values");
    }

}

size_t SerialComImpl::read(char* buffer, size_t bufSize)
{
    DWORD read = -1;

    ReadFile(fileHandle, buffer, static_cast<DWORD>(bufSize), &read, NULL);

    return static_cast<size_t>(read);
}

size_t SerialComImpl::write(const char* buffer, size_t bufSize)
{
    DWORD write = -1;
    WriteFile(fileHandle,buffer,static_cast<DWORD>(bufSize),&write, NULL);

    return write;
}

void SerialComImpl::listComPorts(std::vector<std::string>& portList)
{
	TCHAR lpTargetPath[4096]; // buffer to store the path of the COMPORTS

	for(size_t ind = 1; ind <= 32; ind++)
    {
        std::string port = "COM" + Poco::NumberFormatter::format(ind);

        //std::wstring ws_portName;
        //ws_portName.assign(port.begin(), port.end());

        DWORD dwSize = sizeof(COMMCONFIG);
        LPCOMMCONFIG lpCC = (LPCOMMCONFIG) new char[dwSize];
        if (GetDefaultCommConfig(port.c_str(), lpCC, &dwSize))
            portList.push_back(port);
		else if (QueryDosDevice(port.c_str(), (LPSTR)lpTargetPath, 4096))
			portList.push_back(port);

    }
}

#endif /* WIN32 */
