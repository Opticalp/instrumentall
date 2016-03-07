/**
 * @file	src/DemoModuleB.cpp
 * @date	feb. 2016
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

#include "DemoModuleB.h"
#include "Poco/NumberFormatter.h"

size_t DemoModuleB::refCount = 0;

DemoModuleB::DemoModuleB(ModuleFactory* parent, std::string customName):
                Module(parent)
{
    // poco_information(logger(),"Creating a new demo module B");

    setInternalName("DemoModuleB" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module" + name());

    // ports
    setOutPortCount(outPortCnt);
    addOutPort("outPortA", "demo port that transmits nothing", DataItem::typeInteger, outPortA);

    notifyCreation();

    // if nothing failed
    refCount++;
}
