/**
 * @file	src/modules/devices/GenicamRootFactory.h
 * @date	Janv. 2017
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

#ifndef SRC_MODULES_DEVICES_GENICAMROOTFACTORY_H_
#define SRC_MODULES_DEVICES_GENICAMROOTFACTORY_H_

#include "core/ModuleFactoryBranch.h"

/**
 * GenicamRootFactory
 * 
 * Device factory used to generate GenICam devices.
 *
 * Select chain:
 *  - genTL.cti selection in GenicamRootFactory
 *  - interface selection in GenicamLibFactory
 *  - device selection in GenicamIfaceFactory
 *  - conf file selection in GenicamDeviceFactory
 *  - GenicamConfDeviceFactory creates the device
 */
class GenicamRootFactory: public ModuleFactoryBranch
{
public:
    GenicamRootFactory(ModuleFactory* parent, std::string selector):
        ModuleFactoryBranch(parent, selector, false) { setLogger(name()); }

    std::string name() { return "GenicamRootFactory"; }
    std::string description()
        { return "Factory to create GenICam devices. "
                "The factory chain is: .cti > interface > device"; }

    std::string selectDescription()
        { return "Select the path of the .cti genicam lib "
                "to be used to communicate with the GenICam devices"; }

    std::vector<std::string> selectValueList();

private:
    GenicamRootFactory();

    /**
     * Validate the selector to get an absolute value
     *
     * - check the selector validity: check if it is a .cti genicam lib
     * - Insure that the selector is unique for a given factory (return absolute path)
     */
    std::string validateSelector(std::string selector);

    ModuleFactoryBranch* newChildFactory(std::string selector);
};

#endif /* SRC_MODULES_DEVICES_GENICAMROOTFACTORY_H_ */
