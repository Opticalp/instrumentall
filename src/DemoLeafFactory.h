/**
 * @file	src/DemoLeafFactory.h
 * @date	jan. 2016
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

#ifndef SRC_DEMOLEAFFACTORY_H_
#define SRC_DEMOLEAFFACTORY_H_

#include "ModuleFactoryBranch.h"

/**
 * DemoLeafFactory
 *
 * Very simple module factory to demonstrate ModuleFactory usage
 * in the case of a leaf factory (that generates Modules)
 *
 * Child of DemoBranchFactory, itself child of DemoRootFactory
 */
class DemoLeafFactory: public ModuleFactoryBranch
{
public:
    DemoLeafFactory(ModuleFactory* parent, std::string selector):
        ModuleFactoryBranch(parent, selector) { setLogger(name()); }
    virtual ~DemoLeafFactory() { }

    std::string name() { return std::string("Demo" + getSelector() + "Factory"); }
    const char * description() const
    {
        return "Example code for a leaf module factory. "
                "Please use create() to create a module. ";
    }

    size_t countRemain() { return 1; }

private:
    Module* newChildModule(std::string customName);

};

#endif /* SRC_DEMOLEAFFACTORY_H_ */
