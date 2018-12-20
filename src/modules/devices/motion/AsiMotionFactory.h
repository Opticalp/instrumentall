/**
 * @file	src/modules/devices/AsiMotionFactory.h
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

#ifndef SRC_ASIMOTIONFACTORY_H_
#define SRC_ASIMOTIONFACTORY_H_

#include "core/ModuleFactoryBranch.h"
#include "tools/comm/serial/SerialCom.h"

/**
 * AsiMotionFactory
 *
 * Create AsiMotion modules to control ASI Motion stages.
 * The presence of the stage is not checked at the creation.
 * You have to create the module, and then open the port of the module.
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
                "to interface an ASI Motion stage. "
                "The ASI Motion stage presence can not "
                "be checked at this step. ";
    }

    size_t countRemain();

private:
    Module* newChildModule(std::string customName);
    
    SerialCom serial; ///< serial communication object
    
    bool tiger;
};

#endif /* SRC_ASIMOTIONFACTORY_H_ */
