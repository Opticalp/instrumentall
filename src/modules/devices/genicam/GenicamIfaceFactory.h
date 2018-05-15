/**
 * @file	src/modules/devices/genicam/GenicamIfaceFactory.h
 * @date	Jan. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_DEVICES_GENICAM_GENICAMIFACEFACTORY_H_
#define SRC_MODULES_DEVICES_GENICAM_GENICAMIFACEFACTORY_H_

#include "core/ModuleFactoryBranch.h"

#include "GenTLLib.h"

/**
 * GenicamIfaceFactory
 * 
 * Select chain:
 *  - genTL.cti selection in GenicamRootFactory -- DONE --
 *  - interface selection in GenicamLibFactory -- DONE --
 *  - device selection in GenicamIfaceFactory
 *  - conf file selection in GenicamDeviceFactory
 *  - GenicamConfDeviceFactory creates the device
 */
class GenicamIfaceFactory: public ModuleFactoryBranch
{
public:
    /**
     * Constructor
     *
     * Open interface
     * Launch devices discovery using deviceDiscover()
     */
    GenicamIfaceFactory(GenTLLib* genTL, ModuleFactory* parent, std::string selector);

    std::string name() { return "GenicamIfaceFactory"; }
    std::string description()
        { return "Factory to create GenICam device factory, given a genicam interface. "; }

    std::string selectDescription()
        { return "Select the genicam device "
                "to be used to create the leaf device factory"; }

    /**
     * Find the available genicam devices and expose it
     */
    std::vector<std::string> selectValueList();

private:
    GenicamIfaceFactory();

    void terminate();

    ModuleFactoryBranch* newChildFactory(std::string selector);

    /**
     * Discover the devices
     */
    void discover();

    /**
     * Retrieve extended info about a GenTL device
     */
    void genTLDeviceExInfo(char* pDeviceID);

    /**
     * Elementary info for genTLDeviceExInfo
     */
    void genTLDeviceExInfoElem( std::string title,
        char *pDeviceID,
        GenTL::DEVICE_INFO_CMD infoCmd,
        char* pBuffer, size_t bufferSize);

    std::vector<std::string> devices;

    GenTLLib* mGenTL; ///< Interface to the shared lib
    GenTL::IF_HANDLE _TLhInterface; ///< handle on the GenTL interface module
};

#endif /* SRC_MODULES_DEVICES_GENICAM_GENICAMIFACEFACTORY_H_ */
