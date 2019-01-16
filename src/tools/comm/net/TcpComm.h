/**
 * @file	src/tools/comm/net/TcpComm.h
 * @date	May 2018
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

#ifndef SRC_TOOLS_COMM_NET_TCPCOMM_H_
#define SRC_TOOLS_COMM_NET_TCPCOMM_H_

#include "Poco/Logger.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/IPAddress.h"

/**
 * Manage TCP socket connection
 *
 * @todo thread safety
 */
class TcpComm
{
public:
	TcpComm(Poco::Net::IPAddress remoteAddress, int remotePort);
	virtual ~TcpComm();

	/**
	 * Open the communication
	 */
	void open(); 

	/**
	 * Close the communication
	 */
	void close();

	/**
	 * Check if the port is open and valid
	 *
	 * @throw Poco::IOException if not valid
	 */
	void checkOpen()
	{
		if (!isOpen())
			throw Poco::IOException(deviceName() + " is not open");
	}

	/**
	 * Check if the port is open
	 */
	bool isOpen() { return opened; }

	/**
	 * Set the command delimiter
	 *
	 * to be appended at the end of the commands
	 */
	void setDelimiter(char delim) { delimiter = delim; }

	/**
	 * Read the response
	 *
	 * @param maxCharCnt maximum number of characters to read.
	 * if maxCharCnt is zero, read until timeout
	 */
	std::string read(size_t maxCharCnt = 0);

	/**
	  * Write a command
	  *
	  * add the delimiter at the end
	  */
	void write(std::string command);

	/**
	 * Send the given query
	 *
	 * @return response
	 */
	std::string sendQuery(std::string query);

	/**
	 * Send the given query and wait that the given substring is present in the response
	 *
	 * @return response
	 */
	std::string sendQueryCheckResp(std::string query, std::string respSubStr);

	std::string deviceName()
	{
		return tcpSocket.address().toString();
	}

    void setLogger(std::string loggerName);

private:
	char delimiter;

	Poco::Net::SocketAddress sa;
	Poco::Net::StreamSocket tcpSocket;

	bool opened;

    Poco::Logger& logger();
    Poco::Logger* _pLogger; ///< local logger
};

#endif /* SRC_TOOLS_COMM_NET_TCPCOMM_H_ */
