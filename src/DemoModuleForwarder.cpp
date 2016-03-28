/**
 * @file	src/DemoModuleForwarder.cpp
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

#include "DataAttributeIn.h"
#include "DataAttribute.h"

#include "InPort.h"
#include "OutPort.h"

#include "Poco/NumberFormatter.h"

size_t DemoModuleForwarder::refCount = 0;

DemoModuleForwarder::DemoModuleForwarder(ModuleFactory* parent, std::string customName):
        Module(parent, customName)
{
    // poco_information(logger(),"Creating a new DemoModuleForwarder");

    setInternalName("DemoModuleForwarder" + Poco::NumberFormatter::format(refCount));
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

void DemoModuleForwarder::process(InPortLockUnlock& inPortsAccess)
{
    DataAttributeIn attr;

    int* pData;

    // try to acquire the data
    // It should not be a problem since this is the only input data
    // then, if the task was launched, it is probably that this is due
    // to a push.
    if (!inPortsAccess.tryData<int>(inPortA, pData, &attr))
    {
        poco_information(logger(),
                "DemoModuleForwarder::runTask(): "
                "failed to acquire the input data lock. "
                "Data is probably not up to date. ");

        return; // data not up to date
    }

    int tmpData = *pData;
    DataAttributeOut outAttr = attr;

    inPortsAccess.releaseData(inPortA);

    int* pOutData;

    // try to acquire the output data lock
    while (!getOutPorts()[outPortA]->tryData<int>(pOutData))
    {
        poco_information(logger(),
                "DemoModuleForwarder::runTask(): "
                "failed to acquire the output data lock");

        if (sleep(TIME_LAPSE))
        {
            poco_notice(logger(),
                    "DemoModuleForwarder::runTask(): cancelled!");
            return;
        }
    }

    *pOutData = tmpData;
    getOutPorts()[outPortA]->notifyReady(outAttr);

    poco_information(logger(), "DemoModuleForwarder::runTask(): "
            + Poco::NumberFormatter::format(tmpData) + " was forwarded.");
}
