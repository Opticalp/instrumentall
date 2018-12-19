/**
 * @file	src/modules/devices/MotionFactory.h
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

#ifndef SRC_MOTIONFACTORY_H_
#define SRC_MOTIONFACTORY_H_

#include "core/ModuleFactoryBranch.h"

/**
 * MotionFactory
 *
 * Module factory for the motion devices
 */
class MotionFactory: public ModuleFactoryBranch
{
public:
    MotionFactory(ModuleFactory* parent, std::string selector):
        ModuleFactoryBranch(parent, selector, false) { setLogger(name()); }

    std::string name() { return "MotionFactory"; }
    std::string description()
        { return "Factory to create modules that interface motion devices"; }

    std::string selectDescription()
        { return "Select the interface to access to the motion device"; }

    std::vector<std::string> selectValueList();

private:
    ModuleFactoryBranch* newChildFactory(std::string selector);

    enum ifaceType {
        undefinedIface = -1,
        nicIface,
        serialIface,
        ifaceTypeCnt
    };

    /**
     * Check which interfaces are present
     *
     * @param[out] list cleared before storing the results
     */
    void findInterfaces(std::vector<std::string>& list);

    /**
     * Check which NICs are present
     *
     * Present implementation returns only IPV4 NICs
     *
     * @param[out] list NICs are pushed back into this parameter
     */
    void findNics(std::vector<std::string>& nicNamelist);

    /**
     * Check which serial interfaces are present
     *
     * Present implementation returns only free choice
     *
     * @param[out] list serial coms are pushed back into this parameter
     */
    void findSerial(std::vector<std::string>& serialPortNamelist);

    /**
     * check if an interface is still present
     *
     * @return its type
     */
    ifaceType hasInterface(const std::string ifaceName);

    /**
     * Check if the given interface name is a valid NIC
     */
    bool hasNic(const std::string ifaceName);

    /**
     * Check if the given interface name is a valid serial comm port
     */
    bool hasSerial(const std::string ifaceName);
};

#endif /* SRC_MOTIONFACTORY_H_ */
