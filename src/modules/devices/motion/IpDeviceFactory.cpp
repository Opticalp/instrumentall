/**
 * @file	src/modules/devices/motion/IpDeviceFactory.cpp
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

#include "IpDeviceFactory.h"
#include "Meca500Factory.h"

#include "Poco/NumberFormatter.h"

using namespace Poco::Net;

IpDeviceFactory::IpDeviceFactory(ModuleFactory* parent, std::string selector):
    ModuleFactoryBranch(parent, selector, false),
	comm(nic())
{
    setLogger(name());
    comm.setLogger(logger().name() + ".udpComm");
}

NetworkInterface IpDeviceFactory::nic()
{
    NetworkInterface::List nicList = NetworkInterface::list();
    IPAddress wantedIP(getSelector());

    for (NetworkInterface::List::iterator it = nicList.begin(),
            ite = nicList.end(); it != ite; it++)
        if (it->address() == wantedIP)
            return *it;

    throw Poco::RuntimeException("LightControllerIpDeviceFactory",
            "unknown NIC");
}

std::vector<std::string> IpDeviceFactory::selectValueList()
{
    std::vector<std::string> list;

    findDevices(list);

    return list;
}

ModuleFactoryBranch* IpDeviceFactory::newChildFactory(std::string selector)
{
    switch (hasDevice(selector))
    {
    case meca500Dev:
        return meca500candidates[selector];
    case undefinedDev:
    default:
        poco_bugcheck_msg("Create: unknown device");
        throw Poco::BugcheckException();
    }
}

void IpDeviceFactory::findDevices(std::vector<std::string>& devList)
{
    devList.clear();
    findMeca500(devList);
    // TODO: check uniqueness?
}

IpDeviceFactory::DevType IpDeviceFactory::hasDevice(std::string selector)
{
    IPAddress IP;
    DevType brand;
    parseSelector(selector, IP, brand);

    if (hasMeca500(IP))
        return meca500Dev;

    return undefinedDev;
}

void IpDeviceFactory::findMeca500(std::vector<std::string>& devList)
{
	if (nic().isLoopback())
	{
		poco_warning(logger(), "NIC " + nic().adapterName() + 
			" is loopback. Skip. ");
		return;
	}

	if (nic().isPointToPoint())
	{
		poco_notice(logger(), "NIC " + nic().adapterName() +
			" is point to point. ");
	}

	NetworkInterface::AddressList addrList = nic().addressList();
	for (NetworkInterface::AddressList::iterator it = addrList.begin(),
		ite = addrList.end(); it != ite; it++)
	{
		poco_information(logger(), "analyzing " + it->get<0>().toString());

		if (it->get<0>().family() != AddressFamily::IPv4)
		{
			poco_warning(logger(), "NIC " + nic().adapterName() +
				" address: " + it->get<0>().toString() +
				" is not IPV4. Skip. ");
			continue;
		}

		std::string addrPrefix = it->get<0>().toString();

		// remove last token
		size_t ptPos = addrPrefix.rfind('.');
		addrPrefix.erase(ptPos);
		
		// ping all devices in subnet (but not the broadcast address). 
		for (int ipMinor = 1; ipMinor < 256; ipMinor++)
		{
			IPAddress address = IPAddress(addrPrefix + "." + Poco::NumberFormatter::format(ipMinor));

			if (address == it->get<0>())
				continue;

			poco_information(logger(), "Check " + address.toString());

			try
			{
				if (hasMeca500(address))
				{
					poco_information(logger(), "meca500 found. ");
					devList.push_back(buildSelector(address, meca500Dev));
				}
			}
			catch (Poco::Exception& e)
			{
				poco_warning(logger(), "Meca500 discover failed on " +
					address.toString() + ": " + e.displayText());
			}
		}
	}
}

bool IpDeviceFactory::hasMeca500(IPAddress IP)
{
	std::string selector;
	selector = buildSelector(IP, meca500Dev);

	if (meca500candidates.count(selector))
		// NOTE: could check that the socket is stil opened
		return true;

	try
	{
		meca500candidates[selector]=
				new Meca500Factory(this, selector);
		return true;
	}
	catch (Poco::Exception& e)
	{
		poco_warning( logger(), 
			"Not able to open the control port: " + 
			e.displayText()  );
		return false;
	}
}

std::string IpDeviceFactory::buildSelector(IPAddress IP,
        DevType brand)
{
    std::string sel;
    sel.append(IP.toString());
    return sel;
}

bool IpDeviceFactory::parseSelector(std::string selector,
        IPAddress& IP, DevType& brand)
{
    brand = undefinedDev;
    try
    {
        IP = IPAddress(selector);
        return true;
    }
    catch (Poco::Exception& e)
    {
        poco_warning(logger(), e.displayText());
        return false;
    }
}

IPAddress IpDeviceFactory::parseSelector(std::string selector)
{
	IPAddress retIP;
	DevType brand;

	if (parseSelector(selector, retIP, brand))
		return retIP;
	else
		throw Poco::RuntimeException(name(),
				"Not able to retrieve the IP address from the selector");
}
