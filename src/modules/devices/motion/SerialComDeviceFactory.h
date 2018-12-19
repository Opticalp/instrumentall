/**
 * @file	src/modules/devices/motion/SerialComDeviceFactory.h
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

#ifndef SRC_MODULES_DEVICES_MOTION_SERIALCOMDEVICEFACTORY_H_
#define SRC_MODULES_DEVICES_MOTION_SERIALCOMDEVICEFACTORY_H_

#include "core/ModuleFactoryBranch.h"
#include "tools/serial/SerialComm.h"

/**
 * Branch factory used to select the right serial comm device
 *
 * to build a motion module.
 * 
 * Created from a Motion Factory in case of serial comm interface selection
 */
class SerialComDeviceFactory: public ModuleFactoryBranch
{
public:
    SerialComDeviceFactory(ModuleFactory* parent, std::string selector);
    virtual ~SerialComDeviceFactory() { }

    std::string name() { return "MotionSerialComDeviceFactory"; }
    std::string description()
        { return "Factory to create a specific motion device factory. "
                "This factory works for serial comm devices. "; }

    std::string selectDescription()
        { return "Select the type of the target motion device. "
                "To be used to create the leaf device factory"; }

    /**
     * Expose the known motion devices with serial comm
     */
    std::vector<std::string> selectValueList();

private:
    ModuleFactoryBranch* newChildFactory(std::string selector);
};

#endif /* SRC_MODULES_DEVICES_MOTION_SERIALCOMDEVICEFACTORY_H_ */
