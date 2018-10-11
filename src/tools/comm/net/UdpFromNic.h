/**
 * @file	src/tools/comm/net/UdpFromNic.h
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

#ifndef SRC_TOOLS_COMM_NET_UDPFROMNIC_H_
#define SRC_TOOLS_COMM_NET_UDPFROMNIC_H_

#include "Poco/Logger.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetworkInterface.h"

/**
 * UdpFromNic
 *
 * Manage UDP communication from a given NIC.
 * The implicit copy assignment operator can be safely used.
 */
class UdpFromNic
{
public:
	UdpFromNic(Poco::Net::NetworkInterface NIC):
		nic(NIC), _pLogger(&Poco::Logger::get("udpComm")) { }

    /**
     * Broadcast UDP message to the given port, from the given port
     */
    void sendBroadcast(std::string msg, int srcPort, int tgtPort);

    /**
     * Send a standard datagram to the given target, from the source port srcPort
     */
    void sendDatagram(std::string msg, int srcPort, Poco::Net::SocketAddress target);

    /**
     * Get standard datagram on the given port
     *
     * @return false on timeout
     */
    bool getDatagram(int tgtPort, Poco::Net::IPAddress& source, std::string& response);

    void setLogger(std::string loggerName);

private:
	Poco::Net::NetworkInterface nic; ///< NIC to use for this UDP connexion

    Poco::Logger& logger();
    Poco::Logger* _pLogger; ///< local logger
};

#endif /* SRC_TOOLS_COMM_NET_UDPFROMNIC_H_ */
