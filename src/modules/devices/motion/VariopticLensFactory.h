/**
 * @file	src/modules/devices/motion/VariopticLensFactory.h
 * @date	Apr. 2020
 * @author	PhRG
 */

/*
 Copyright (c) 2020 Ph. Renaud-Goud

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

#ifndef SRC_MODULES_DEVICES_MOTION_VARIOPTICLENSFACTORY_H_
#define SRC_MODULES_DEVICES_MOTION_VARIOPTICLENSFACTORY_H_

#include "core/ModuleFactoryBranch.h"
#include "tools/comm/serial/SerialCom.h"

/**
 * VariopticLensFactory
 *
 * Create VariopticLens modules to control Varioptic liquid lenses.
 */
class VariopticLensFactory: public ModuleFactoryBranch
{
public:
    VariopticLensFactory(ModuleFactory* parent, std::string selector);
    virtual ~VariopticLensFactory() { }

    std::string name() { return "VariopticLensFactory"; }
    std::string description()
    {
        return "Leaf factory to create a module "
                "to interface a Varioptic liquid lens. ";
    }

    size_t countRemain();

private:
    Module* newChildModule(std::string customName);
    
    SerialCom serial; ///< serial communication object
};

#endif /* SRC_MODULES_DEVICES_MOTION_VARIOPTICLENSFACTORY_H_ */
