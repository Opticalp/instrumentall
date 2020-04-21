/**
 * @file	src/tools/comm/net/TcpComm.cpp
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

#include "TcpComm.h"
#include "tools/comm/Decorated.h"

#include "Poco/NumberFormatter.h"

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetException.h"

using namespace Poco::Net;

#define TIMEOUT_MS 500

// how many times timeout before aborting response awaiting
#define TIMEOUT_COUNTER_LIMIT 20

TcpComm::TcpComm(Poco::Net::IPAddress remoteAddress, int remotePort) :
	delimiter('\r'), 
	hasDelimiter(true),
	sa(remoteAddress, remotePort), 
	opened(false),
	_pLogger(&Poco::Logger::get("comm.tcp." + sa.toString()))
{
}

TcpComm::~TcpComm()
{
	try
	{
		close();
	}
	catch (...)
	{
	}
}

void TcpComm::noDelimiter()
{
	hasDelimiter = false;
}

void TcpComm::setDelimiter(char delim)
{
	delimiter = delim;
	hasDelimiter = true;
}

void TcpComm::open()
{
	try
	{
		tcpSocket.connect(sa);
		tcpSocket.setReuseAddress(true);
		tcpSocket.setReusePort(true);
		tcpSocket.setKeepAlive(true);
		tcpSocket.setSendTimeout(TIMEOUT_MS * 1000);
		tcpSocket.setReceiveTimeout(TIMEOUT_MS * 1000);
	}
	catch (...)
	{
		tcpSocket.shutdown();
		tcpSocket.close();
		throw;
	}

	opened = true;
}

void TcpComm::close()
{
	try
	{
		tcpSocket.shutdown();
		tcpSocket.close();
	}
	catch (Poco::Exception& e)
	{
		poco_warning(logger(), "The TCP socket " + deviceName()
			+ " was not cleanly closed: " + e.displayText());
	}

	opened = false;
}

std::string TcpComm::read(size_t maxCharCnt)
{
	checkOpen();

	std::string response, accu;

	size_t length = 4096;
	if (maxCharCnt)
		length = maxCharCnt;

	char* buffer = new char[length];

	bool timeout = true;

	if (maxCharCnt == 0) // receive until timeout
		timeout = false;

	do
	{
		try
		{
			int len = tcpSocket.receiveBytes(buffer, length);

			if (len)
			{
				response.assign(buffer, len);

				poco_information(logger(),
					Poco::NumberFormatter::format(len) + " bytes received: \n" +
					decoratedCommandKeep(response));
			}
			else
			{
				close();
				throw Poco::Net::ConnectionResetException("Connection reset by peer");
			}

			for (std::string::iterator it = response.begin(),
				ite = response.end(); it != ite; it++)
			{
				if (hasDelimiter && (*it == delimiter))
					*it = '\n';
			}

			accu += response;
		}
		catch (Poco::TimeoutException)
		{
			if (maxCharCnt)
			{
				delete[] buffer;
				throw;
			}
			else
			{
				timeout = true;
			}
		}
		catch (...)
		{
			delete[] buffer;
			throw;
		}
	} while (!timeout);

	delete[] buffer;
	return accu;
}

void TcpComm::write(std::string command)
{
	checkOpen();

	size_t cmdSize = command.size();

	if (hasDelimiter)
	{
		command += delimiter;
		cmdSize++;
	}

	try
	{
		int ret = tcpSocket.sendBytes(command.c_str(), cmdSize);
		if (ret != cmdSize)
			throw Poco::IOException(
				std::string("TcpComm " + shortName()),
				"unable to write the full data (" 
				+ Poco::NumberFormatter::format(cmdSize) +
				" bytes) to the port. Returned: "
				+ Poco::NumberFormatter::format(ret));

		poco_information(logger(),
			Poco::NumberFormatter::format(cmdSize) + " bytes sent: \n" +
			decoratedCommandKeep(command, cmdSize));
	}
	catch (Poco::Exception& e)
	{
		poco_error(logger(), command + " [## SEND ERROR ##]: " + e.displayText());
	}
}

std::string TcpComm::sendQuery(std::string query, size_t maxCharCnt)
{
	write(query);
	return read(maxCharCnt);
}

std::string TcpComm::sendQueryCheckResp(std::string query,
	std::string respSubStr, size_t maxCharCnt)
{
	std::string resp;

	try
	{
		resp = sendQuery(query, maxCharCnt);
	}
	catch (Poco::TimeoutException&)
	{
		poco_warning(logger(), "response awaiting timed out once. ");
	}

	int timeoutCnter = 0;
	while (resp.find(respSubStr) == std::string::npos)
	{
		try
		{
			resp += read(maxCharCnt);
		}
		catch (Poco::TimeoutException&)
		{
			poco_warning(logger(), "response awaiting timed out (#"
				+ Poco::NumberFormatter::format(timeoutCnter)
				+ ")");
			timeoutCnter++;
			if (timeoutCnter > TIMEOUT_COUNTER_LIMIT)
				throw;
		}
	}

	return resp;
}

void TcpComm::setLogger(std::string loggerName)
{
    _pLogger = &Poco::Logger::get(loggerName);
}

Poco::Logger& TcpComm::logger()
{
    poco_check_ptr (_pLogger);
    return *_pLogger;
}
