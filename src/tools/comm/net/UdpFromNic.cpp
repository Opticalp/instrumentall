/**
 * @file	src/tools/comm/net/UdpFromNic.cpp
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

#include "UdpFromNic.h"
#include "tools/comm/Decorated.h"

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/NumberFormatter.h"

using namespace Poco::Net;

#define UDP_LISTEN_TIMEOUT_MS 500

void UdpFromNic::sendBroadcast(std::string msg, int srcPort, int tgtPort)
{
    SocketAddress sourceAddress(nic.address(), srcPort);
    SocketAddress broadcastTarget(nic.broadcastAddress(), tgtPort);

    poco_information(logger(),
            "Broadcasting " + decoratedCommand(msg) + " to "
                     + nic.broadcastAddress().toString());

    DatagramSocket dgs(sourceAddress, true);
    dgs.setBroadcast(true);
    dgs.sendTo(msg.c_str(), msg.size(), broadcastTarget);
}


void UdpFromNic::sendDatagram(std::string msg, int srcPort,
        Poco::Net::SocketAddress target)
{
    SocketAddress src(nic.address(), srcPort);
    DatagramSocket dgs(src, true);

    poco_information(logger(),
            "Sending " + decoratedCommand(msg) + " to "
                     + target.toString());

    dgs.sendTo(msg.data(), msg.size(), target);
}

bool UdpFromNic::getDatagram(int tgtPort, Poco::Net::IPAddress& source,
        std::string& response)
{
    Poco::Net::SocketAddress sa(IPAddress(), tgtPort);
    Poco::Net::DatagramSocket dgs(sa);

    dgs.setReceiveTimeout(Poco::Timespan(0,UDP_LISTEN_TIMEOUT_MS*1000));

    poco_information(logger(), "Waiting for message on port "
            + Poco::NumberFormatter::format(tgtPort));

    char buffer[1024];

    try
    {
        Poco::Net::SocketAddress sender;
        int n = dgs.receiveFrom(buffer, sizeof(buffer)-1, sender);

        if (n)
        {
            response = std::string(buffer, n);

            poco_information(logger(), "received response (from "
                    + sender.host().toString() + "): \n"
                    + decoratedCommandKeep(response));

            if (sender.host() != Poco::Net::IPAddress("0.0.0.0"))
            {
                source = sender.host();
                return true;
            }
            else
            {
                poco_warning(logger(),"IP Light controller device may not be configured."
                    " Please set its IP first using a DHCP server.");
            }
        }
        else
        {
            poco_warning(logger(), "Received datagram socket (from "
                    + sender.host().toString() + ") without data? ");
        }
    }
    catch (Poco::TimeoutException&)
    {
    }

    return false;
}

void UdpFromNic::setLogger(std::string loggerName)
{
    _pLogger = &Poco::Logger::get(loggerName);
}

Poco::Logger& UdpFromNic::logger()
{
    poco_check_ptr (_pLogger);
    return *_pLogger;
}
