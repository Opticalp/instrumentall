/**
 * @file	src/modules/devices/MotionFactory.cpp
 * @date	Apr. 2018
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

#include "MotionFactory.h"
#include "motion/IpDeviceFactory.h"
#include "motion/SerialComDeviceFactory.h"

#include "Poco/NumberFormatter.h"

std::vector<std::string> MotionFactory::selectValueList()
{
    std::vector<std::string> list;

    findInterfaces(list);

    return list;
}

ModuleFactoryBranch* MotionFactory::newChildFactory(std::string selector)
{
    switch (hasInterface(selector))
    {
    case nicIface:
        return new IpDeviceFactory(this, selector);
    case serialIface:
        return new SerialComDeviceFactory(this, selector);
    case undefinedIface:
    default:
        poco_bugcheck_msg("Create: unknown interface");
        throw Poco::BugcheckException();
    }
}

void MotionFactory::findInterfaces(std::vector<std::string>& list)
{
    list.clear();
    findNics(list);
    findSerial(list);
}

MotionFactory::ifaceType MotionFactory::hasInterface(const std::string ifaceName)
{
    if (hasNic(ifaceName))
        return nicIface;
    else if (hasSerial(ifaceName))
        return serialIface;

    return undefinedIface;
}

#include "Poco/Net/NetworkInterface.h"

using namespace Poco::Net;

void MotionFactory::findNics(std::vector<std::string>& nicNameList)
{
	NetworkInterface::List nicList = NetworkInterface::list();

    for (NetworkInterface::List::iterator it = nicList.begin(),
            ite = nicList.end(); it != ite; it++)
    {
        IPAddress address = it->address();
        if (address.family() == IPAddress::IPv4)
            nicNameList.push_back(it->address().toString());
            // n.b. trailing NIC name is appended at the end of the address
            // after "%" under linux. We should be able to ignore this.
    }

}

bool MotionFactory::hasNic(const std::string ifaceName)
{
	NetworkInterface::List nicList = NetworkInterface::list();

    for (NetworkInterface::List::iterator it = nicList.begin(),
            ite = nicList.end(); it != ite; it++)
		try
		{ 
			if (it->address() == IPAddress(ifaceName))
				return true;
		}
		catch (Poco::Exception &e)
		{
			poco_information(logger(), ifaceName + ": " + e.displayText());
		}

    return false;
}


#include "tools/comm/serial/SerialCom.h"

void MotionFactory::findSerial(std::vector<std::string>& serialPortNamelist)
{
    SerialCom::listComPorts(serialPortNamelist);
}

bool MotionFactory::hasSerial(const std::string ifaceName)
{
    std::vector<std::string> serialList;
    
    findSerial(serialList);
	for (std::vector<std::string>::iterator it = serialList.begin(),
		ite = serialList.end(); it != ite; it++)
	{
		if (it->compare(ifaceName) == 0)
		{
			poco_information(logger(), "interface found: " + *it);
			return true;
		}
		else
			poco_information(logger(), ifaceName + " is not " + *it);
	}
                
    return false;
}
