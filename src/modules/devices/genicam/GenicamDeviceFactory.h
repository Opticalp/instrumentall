/**
 * @file	src/modules/devices/genicam/GenicamDeviceFactory.h
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

#ifndef SRC_MODULES_DEVICES_GENICAM_GENICAMDEVICEFACTORY_H_
#define SRC_MODULES_DEVICES_GENICAM_GENICAMDEVICEFACTORY_H_

#include "core/ModuleFactoryBranch.h"

#include "GenTLLib.h"

/**
 * GenicamDeviceFactory
 *
 * Select chain:
 *  - genTL.cti selection in GenicamRootFactory -- DONE --
 *  - interface selection in GenicamLibFactory -- DONE --
 *  - device selection in GenicamIfaceFactory -- DONE --
 *  - conf file selection in GenicamDeviceFactory
 *  - GenicamConfDeviceFactory creates the device
 *
 * @par Version hef 2.0.0-beta.2 and later
 * This factory is an intermediate factory. The config file has to be
 * selected to go to the leaf factory
 */
class GenicamDeviceFactory: public ModuleFactoryBranch
{
public:
    GenicamDeviceFactory(GenTLLib* genTL, GenTL::IF_HANDLE TLhInterface,
            ModuleFactory* parent, std::string selector);

    std::string name() { return "GenicamDeviceFactory"; }
    std::string description()
    {
        return "Device genicam factory. "
                "The genicam camera is selected. "
                "Select now a config file to define the exposed parameters";
    }

    std::string selectDescription()
    {
        return "Select the yaml config file "
                "to be used to expose the GenAPI parameters "
                "in the camera module. \"None\" means that no "
                "parameter will be exposed. ";
    }

    /**
     * Find the available genicam devices and expose it
     */
    std::vector<std::string> selectValueList();

private:
    GenicamDeviceFactory();

    std::string validateSelector(std::string selector);

    void terminate();

    ModuleFactoryBranch* newChildFactory(std::string selector);

    GenTLLib* mGenTL; ///< Interface to the shared lib
    GenTL::IF_HANDLE _TLhInterface; ///< handle on the GenTL interface module
    GenTL::DEV_HANDLE _TLhDevice; ///< handle on the GenTL device module
};

#endif /* SRC_MODULES_DEVICES_GENICAM_GENICAMDEVICEFACTORY_H_ */
