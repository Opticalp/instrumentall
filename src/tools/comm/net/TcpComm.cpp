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
	delimiter('\r'), sa(remoteAddress, remotePort), opened(false)
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
	std::string response;

	char* buffer = new char[maxCharCnt];

	try
	{
		int len = tcpSocket.receiveBytes(buffer, maxCharCnt);

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
			if (*it == delimiter)
				*it = '\n';
		}
	}
	catch (...)
	{
		delete[] buffer;
		throw;
	}

	delete[] buffer;
	return response;
}

void TcpComm::write(std::string command)
{
	int length = command.size();
	try
	{
		tcpSocket.sendBytes((command + delimiter).c_str(), length + 1);

		poco_information(logger(),
			Poco::NumberFormatter::format(length) + " bytes sent: \n" +
			decoratedCommandKeep((command + delimiter), length + 1));
	}
	catch (Poco::Exception& e)
	{
		poco_error(logger(), command + " [## SEND ERROR ##]: " + e.displayText());
	}
}

std::string TcpComm::sendQuery(std::string query)
{
	int length;

	length = query.size();
	try
	{

		tcpSocket.sendBytes((query + delimiter).c_str(), length + 1);

		poco_information(logger(),
			Poco::NumberFormatter::format(length) + " bytes sent: \n" +
			decoratedCommandKeep((query + delimiter), length + 1));
	}
	catch (Poco::Exception& e)
	{
		poco_error(logger(), query + " [## SEND ERROR ##]: " + e.displayText());
	}

	return read();
}

std::string TcpComm::sendQueryCheckResp(std::string query,
	std::string respSubStr)
{
	std::string resp;

	try
	{
		resp = sendQuery(query);
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
			resp += read();
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
