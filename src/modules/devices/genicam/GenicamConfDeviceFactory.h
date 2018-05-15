/**
 * @file	src/modules/devices/genicam/GenicamConfDeviceFactory.h
 * @date	May 2017
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

#ifndef SRC_MODULES_DEVICES_GENICAM_GENICAMCONFDEVICEFACTORY_H_
#define SRC_MODULES_DEVICES_GENICAM_GENICAMCONFDEVICEFACTORY_H_

#ifdef HAVE_GENAPI

#include "core/ModuleFactoryBranch.h"

#include "GenTLLib.h"

/**
 * GenicamConfDeviceFactory
 * 
 * Select chain:
 *  - genTL.cti selection in GenicamRootFactory -- DONE --
 *  - interface selection in GenicamLibFactory -- DONE --
 *  - device selection in GenicamIfaceFactory -- DONE --
 *  - conf file selection in GenicamDeviceFactory -- DONE --
 *  - GenicamConfDeviceFactory creates the device
 */
class GenicamConfDeviceFactory: public ModuleFactoryBranch
{
public:
    GenicamConfDeviceFactory(GenTLLib* genTL,
            GenTL::IF_HANDLE TLhInterface,
            GenTL::DEV_HANDLE TLhDevice,
            ModuleFactory* parent, std::string selector);

    std::string name() { return "GenicamConfDeviceFactory"; }
    std::string description()
    {
        return "Leaf genicam factory. "
                "Create a genicam camera from the given lib, interface, device and conf file";
    }

    /**
     * Check if the camera is available.
     *
     * @return 1 if the camera is available. 0 if the camera is already in use.
     */
    size_t countRemain();

private:
    GenicamConfDeviceFactory();

    Module* newChildModule(std::string customName);

    GenTLLib* mGenTL; ///< Interface to the shared lib
    GenTL::IF_HANDLE _TLhInterface; ///< handle on the GenTL interface module
    GenTL::DEV_HANDLE _TLhDevice; ///< handle on the GenTL device module
};

#endif /* HAVE_GENAPI */
#endif /* SRC_MODULES_DEVICES_GENICAM_GENICAMCONFDEVICEFACTORY_H_ */
