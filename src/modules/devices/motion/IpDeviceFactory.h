/**
 * @file	src/modules/devices/motion/IpDeviceFactory.h
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

#ifndef SRC_MODULES_DEVICES_MOTION_IPDEVICEFACTORY_H_
#define SRC_MODULES_DEVICES_MOTION_IPDEVICEFACTORY_H_

#include "core/ModuleFactoryBranch.h"
#include "tools/comm/net/UdpFromNic.h"

#include "Poco/Net/NetworkInterface.h"

/**
 * Branch factory used to select the right IP device
 *
 * to build a motion module.
 * 
 * Created from a Motion Factory in case of NIC interface selection
 */
class IpDeviceFactory: public ModuleFactoryBranch
{
public:
    /**
     * Constructor
     *
     * Retrieve the NIC from the selector
     */
    IpDeviceFactory(ModuleFactory* parent, std::string selector);
    virtual ~IpDeviceFactory() { }

    std::string name() { return "MotionIpDeviceFactory"; }
    std::string description()
        { return "Factory to create a specific motion device factory. "
                "This factory works for IP devices. "; }

    std::string selectDescription()
        { return "Select the IP address of the target motion device. "
                "To be used to create the leaf device factory"; }

    /**
     * Find the available motion IP devices and expose it
     */
    std::vector<std::string> selectValueList();

    Poco::Net::IPAddress parseSelector(std::string selector);

    /**
     * Retrieve the udp comm object
     *
     * Using the implicit copy
     */
    UdpFromNic udpComm() { return comm; }

    /**
     * Simplify the default validator
     *
     * The check is done during creation in newChildFactory
     */
    std::string validateSelector(std::string selector) { return selector; }

private:
    Poco::Net::NetworkInterface nic();

    ModuleFactoryBranch* newChildFactory(std::string selector);

    /**
     * Discover the devices
     *
     * @param[out] devList cleared before discovery
     */
    void findDevices(std::vector<std::string>& devList);

    /**
     * Append the Mecademic Meca500 devices to the list
     */
    void findMeca500(std::vector<std::string>& devList);

    enum DevType {
        undefinedDev = -1,
        meca500Dev,
        devtypeCnt
    };

    /**
     * Build a very simple selector based on IP address
     *
     * Issues can arise if the uniqueness is not guaranteed
     * (see Smartek / Gardasoft compatibility...)
     */
    std::string buildSelector(Poco::Net::IPAddress IP, DevType brand);

    /**
     * Parse the selector as defined by buildSelector
     *
     * @return false if the selector is not recognized
     */
    bool parseSelector(std::string selector, Poco::Net::IPAddress& IP, DevType& brand);

    DevType hasDevice(std::string selector);

    bool hasMeca500(Poco::Net::IPAddress IP);

    UdpFromNic comm;

	/**
	 * Selector to factory candidate
	 */
	std::map<std::string, ModuleFactoryBranch*> meca500candidates;
};

#endif /* SRC_MODULES_DEVICES_MOTION_IPDEVICEFACTORY_H_ */
