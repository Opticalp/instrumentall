/**
 * @file	src/modules/fieldBus/FieldBusFactory.h
 * @date	Nov 2016
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

#ifndef SRC_FIELDBUSFACTORY_H_
#define SRC_FIELDBUSFACTORY_H_

#include "core/ModuleFactory.h"

/**
 * FieldBusFactory
 *
 * Root factory for modules implementing any field bus (ModBus, EtherCAT, ...)
 */
class FieldBusFactory: public ModuleFactory
{
public:
    FieldBusFactory(): ModuleFactory(false) // is not a leaf, is root.
		{ setLogger(name()); }

    virtual ~FieldBusFactory() { }

    std::string name() { return "FieldBusFactory"; }
    std::string description()
    {
        return "Factory for modules that implement "
        		"a field bus";
    }

    std::string selectDescription()
    {
        return "Select the field bus type";
    }

    std::vector<std::string> selectValueList();

private:
    ModuleFactoryBranch* newChildFactory(std::string selector);
};

#endif /* SRC_FIELDBUSFACTORY_H_ */
