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
	comm(reinterpret_cast<IpDeviceFactory*>(parent)->parseSelector(selector), 10000)
{
	setLogger("Meca500Factory");

	// open comm socket or throw error
	comm.setDelimiter('\0');
	comm.open();
	poco_information(logger(), "Control socket connected");

	checkFirstResponse();
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
	try
	{
		std::string response = comm.read(256);

		if (response.find("[3000][Connected to Meca500") == std::string::npos)
		{
			poco_warning(logger(), "Invalid first response. "
				"should be: \"[3000][Connected to Meca500 3_7.0.6]\\0\" "
				"or similar. ");
			throw Poco::IOException("Meca500Factory::checkFirstResponse",
					"invalid response");
		}
	}
	catch (Poco::TimeoutException&)
	{
		poco_warning(logger(), "First response: reception timeout on control port. ");
	}
}

#include "Poco/RegularExpression.h"

bool Meca500Factory::isAvailable()
{
	std::string resp = comm.sendQuery("GetStatusRobot", 256);

	Poco::RegularExpression regexp("\\[2007\\]\\[([01]),([01]),([01]),([01]),([01]),([01]),([01])\\]");
	std::vector<std::string> strVec;
	regexp.split(resp, 0, strVec);

	poco_information(logger(), "status parsing: got "
		+ Poco::NumberFormatter::format(strVec.size() - 1) + " bits");

	if (strVec.size() != 8)
		throw Poco::DataFormatException("not able to parse the Meca500 status");

	if (strVec[1] != "1") // activation
	{
		poco_warning(logger(), "The meca500 is not activated");
		return false;
	}
	if (strVec[2] != "1") // homing
	{
		poco_warning(logger(), "The meca500 is not homed");
		return false;
	}

	return true;
}

Module * Meca500Factory::newChildModule(std::string customName)
{
	return new Meca500(this, customName, comm);
}
