/**
 * @file	src/modules/devices/motion/Meca500Factory.cpp
 * @date	Jan. 2019
 * @author	PhRG - opticalp.fr
 */

 /*
  Copyright (c) 2019 Ph. Renaud-Goud

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

#include "Meca500Factory.h"
#include "Meca500.h"
#include "IpDeviceFactory.h"

#include "tools/comm/Decorated.h"

#include "Poco/NumberFormatter.h"

#include "Poco/Net/NetException.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"

/// socket timeout (ms)
#define TIMEOUT 500

using namespace Poco::Net;

Meca500Factory::Meca500Factory(ModuleFactory* parent, std::string selector) :
	ModuleFactoryBranch(parent, selector), // leaf = true (default)
	sa(reinterpret_cast<IpDeviceFactory*>(parent)->parseSelector(selector), 10000)
{
	setLogger("Meca500Factory");

	// open comm socket or throw error
	tcpSocket.connect(sa);
	try
	{
		tcpSocket.setReuseAddress(true);
		tcpSocket.setReusePort(true);
		tcpSocket.setKeepAlive(true);
		tcpSocket.setSendTimeout(TIMEOUT * 1000);
		tcpSocket.setReceiveTimeout(TIMEOUT * 1000);

		poco_information(logger(), "Control socket connected");

		checkFirstResponse();
	}
	catch (...)
	{
		tcpSocket.shutdown();
		tcpSocket.close();
		throw;
	}
}

size_t Meca500Factory::countRemain()
{
	if (getChildModules().size() || !isAvailable())
		return 0;
	else 
		return 1;
}

void Meca500Factory::checkFirstResponse()
{
	std::string response;

	char buffer[4096];
	int length = 4096;

	try
	{
		int len = tcpSocket.receiveBytes(buffer, length);

		if (len)
		{
			response.assign(buffer, len);

			poco_information(logger(),
				Poco::NumberFormatter::format(len) + " bytes received: \n" +
				decoratedCommandKeep(response));

			if (response.find("[3000][Connected to Meca500") == std::string::npos)
			{
				poco_warning(logger(), "Invalid first response. "
					"should be: \"[3000][Connected to Meca500 3_7.0.6]\\0\" "
					"or similar. ");
				throw Poco::IOException("Meca500Factory::checkFirstResponse",
						"invalid response");
			}
		}
	}
	catch (Poco::TimeoutException&)
	{
		poco_warning(logger(), "First response: reception timeout on control port. ");
	}
}

bool Meca500Factory::isAvailable()
{
	// TODO
	return true;
}

Module * Meca500Factory::newChildModule(std::string customName)
{
	return new Meca500(this, customName, tcpSocket);
}
