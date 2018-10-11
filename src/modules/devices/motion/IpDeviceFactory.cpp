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
#include "modules/GenericLeafFactory.h"
#include "Meca500.h"

#include "tools/comm/Decorated.h"

#include "Poco/NumberFormatter.h"

#include "Poco/Net/NetException.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ICMPClient.h" // ping

 /// socket receive timeout (ms)
#define TIMEOUT 500

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
    NetworkInterface::NetworkInterfaceList nicList = NetworkInterface::list();
    IPAddress wantedIP(getSelector());

    for (NetworkInterface::NetworkInterfaceList::iterator it = nicList.begin(),
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
        return new GenericLeafFactory<Meca500>(
                "Meca500Factory", "Create a Meca500 robot arm module",
                this, selector, true);
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
			std::string address = addrPrefix + "." + Poco::NumberFormatter::format(ipMinor);

			if (IPAddress(address) == it->get<0>())
				continue;

			poco_information(logger(), "ping " + address);

			try
			{
				if (ICMPClient::pingIPv4(address, 1, 48, 128, 10))
				{
					poco_information(logger(), "ping response on " + address);

					//if (hasMeca500(IPAddress(address)))
					//	devList.push_back(buildSelector(sender, meca500Dev));
				}
			}
			catch (Poco::Exception& e)
			{
				poco_warning(logger(), "ping failed: " + e.displayText());
			}
		}
	}
}

//void IpDeviceFactory::findSmartek(std::vector<std::string>& devList)
//{
//    comm.sendBroadcast("Smartek#D\r", 30310, 30311);
//
//    std::string response;
//    IPAddress sender;
//
//    for(int index=0; comm.getDatagram(30310, sender, response); index++)
//    {
//        poco_information(logger(), "Get direct response #"
//                + Poco::NumberFormatter::format(index));
//
//        // no response analysis needed
//        devList.push_back(buildSelector(sender, smartekDev));
//    }
//
//    // response should be:
//    // VV#VI#VN#VA#VT#VL#VF#VF#VF#VF#
////    VV : Version Vendor : #[Vendor]#[Model]#[HardwareVersion]#[FirmwareVersion]
////    VI : Version IP : #[MAC Address]#[IsDHCPMode (D, F)]#[IP Address]#[IP Mask]
////    VN : Status User Name : #[UserName]
////    VA : ID Check mode : #[ID Check Mode]
////    VT : Model Type, Channels Count : #[TypeName]#[ChannelsCount]#[VoltagesCount]#[TriggersCount]
////    VL : Current, Voltage Limits : #[MaxChannelCont]#[MaxChannelStrobe]#[MinVoltage]#[MaxVoltage]
////    VF : DAC Offset : #[ChannelIndex]#[DAC Offset]
//
//// response example with the IPSC4:
////    VV#Smartek#IPSCX#2.0#1.3\r
////    VI#6CD1460101AA#D#00000000#00000000\r
////    VN#STROBE\r
////            \r
////    VA#0\r
////    VT#IPSC4#4#1#4\r
////    VL#1000#10000#5#200\r
////    VF#0#7\r
////    VF#1#2\r
////    VF#2#3\r
////    VF#3#8\r
//}

bool IpDeviceFactory::hasMeca500(IPAddress IP)
{
	// open communication with the device
	StreamSocket tcpSocket; ///< TCP stream socket
	SocketAddress sa(IP, 10001); // monitoring port

	try
	{
		tcpSocket.connect(sa);
	}
	catch (Poco::Exception& e)
	{
		poco_warning(logger(), "Not able to open the monitoring port: " + e.displayText() + ".\n" +
			"No device on this IP, or the robot was not homed yet. ");
		return false;
	}

	tcpSocket.setReuseAddress(true);
	tcpSocket.setReusePort(true);
	tcpSocket.setKeepAlive(true);
	// tcpSocket.setNoDelay(true);
	tcpSocket.setReceiveTimeout(TIMEOUT * 1000);
	//tcpSocket.setSendTimeout(TIMEOUT * 1000); // to check if the connections is still active

	//std::string query("");
	//tcpSocket.sendBytes(query.c_str(), length);

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

			tcpSocket.close();
			return true;
		}
	}
	catch (Poco::TimeoutException&)
	{
		poco_information(logger(), "Timeout on monitoring port. "
			"Please check if the homing routine was ran. ");
	}

	tcpSocket.close();
	return false;
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
