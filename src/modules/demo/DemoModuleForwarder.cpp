/**
 * @file	src/modules/demo/DemoModuleForwarder.cpp
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

#include "DemoModuleForwarder.h"

#include "core/DataAttributeIn.h"
#include "core/DataAttribute.h"

#include "core/InDataPort.h"
#include "core/OutPort.h"

#include "Poco/NumberFormatter.h"

size_t DemoModuleForwarder::refCount = 0;

DemoModuleForwarder::DemoModuleForwarder(ModuleFactory* parent, std::string customName):
        Module(parent, customName)
{
    // poco_information(logger(),"Creating a new DemoModuleForwarder");

    if (refCount)
        setInternalName("DemoModuleForwarder" + Poco::NumberFormatter::format(refCount));
    else
        setInternalName("DemoModuleForwarder");

    setCustomName(customName);
    setLogger("module." + name());

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("inPortA", "data in", DataItem::typeInteger, inPortA);

    addOutPort("outPortA", "forwarded data as is", DataItem::typeInteger, outPortA);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void DemoModuleForwarder::process(int startCond)
{
	if (startCond != allDataStartState)
	{
		poco_information(logger(), name() + ": no input data. Exiting. ");
		return;
	}
	else
	{
		poco_information(logger(),"Input data locked");
	}

    DataAttributeIn attr;
    int* pData;

	readLockInPort(inPortA);
    readInPortData<int>(inPortA, pData);
    readInPortDataAttribute(inPortA, &attr);

    DataAttributeOut outAttr = attr;

    int tmpData; // for verbosity purpose

//    // buffered mode
    tmpData = *pData;
//    releaseInPort(inPortA);

    reserveOutPort(outPortA);

    int* pOutData;
    getDataToWrite<int>(outPortA, pOutData);

//    // buffered mode
//  	*pOutData = tmpData;
//    else

    *pOutData = *pData;

    // not mandatory
    releaseInPort(inPortA);

    notifyOutPortReady(outPortA, outAttr);

    poco_information(logger(), "DemoModuleForwarder::runTask(): "
            + Poco::NumberFormatter::format(tmpData) + " was forwarded.");
}
