/**
 * @file	src/modules/devices/motion/AsiMotionFactory.h
 * @date	Mar 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_DEVICES_MOTION_ASIMOTIONFACTORY_H_
#define SRC_MODULES_DEVICES_MOTION_ASIMOTIONFACTORY_H_

#include "core/ModuleFactoryBranch.h"
#include "tools/comm/serial/SerialCom.h"

/**
 * AsiMotionFactory
 *
 * Create AsiMotion modules to control ASI Motion stages.
 */
class AsiMotionFactory: public ModuleFactoryBranch
{
public:
    AsiMotionFactory(ModuleFactory* parent, std::string selector);
    virtual ~AsiMotionFactory() { }

    std::string name() { return "AsiMotionFactory"; }
    std::string description()
    {
        return "Leaf factory to create a module "
                "to interface an ASI Motion stage. ";
    }

    size_t countRemain();

private:
    Module* newChildModule(std::string customName);
    
    SerialCom serial; ///< serial communication object

	int axes; ///< axes detected during last countRemain() call
    
    bool tiger;
};

#endif /* SRC_MODULES_DEVICES_MOTION_ASIMOTIONFACTORY_H_ */
