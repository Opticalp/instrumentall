/**
 * @file	src/DemoModuleSeqMax.cpp
 * @date	Feb. 2016
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

#include "DemoModuleSeqMax.h"

#include "DataAttributeIn.h"
#include "DataAttribute.h"

#include "InPort.h"
#include "OutPort.h"

#include "Poco/NumberFormatter.h"

size_t DemoModuleSeqMax::refCount = 0;

DemoModuleSeqMax::DemoModuleSeqMax(ModuleFactory* parent, std::string customName):
    Module(parent, customName),
    tmpMax(0) // no special meaning for this value
{
    poco_debug(logger(),"Creating a new DemoModuleSeqMax");

    setInternalName("DemoModuleSeqMax" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module" + name());

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("inPortA", "data sequence", DataItem::typeInteger, inPortA);

    addOutPort("outPortA", "max of data sequence", DataItem::typeInteger, outPortA);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void DemoModuleSeqMax::runTask()
{
    // FIXME: if an exception is raised,
    // the mainMutex unlock is not guaranteed...

    poco_information(logger(), "DemoModuleSeqMax::runTask started. ");

    // We could use a scoped lock here:
    //Poco::Mutex::ScopedLock lock(mainMutex);
    // but then, it could not be possible to check a time out in the lock acquisition

    // try to acquire the mutex
    while (!mainMutex.tryLock(TIME_LAPSE))
    {
        poco_debug(logger(),
                "DemoModuleSeqMax::runTask(): failed to acquire the mutex");

        if (isCancelled())
            return;
    }

    DataAttributeIn attr;

    int* pData;

    // try to acquire the data
    // It should not be a problem since this is the only input data
    // then, if the task was launched, it is probably that this is due
    // to a push.
    if (!getInPorts()[inPortA]->tryData<int>(pData, &attr))
    {
        poco_debug(logger(),
                "DemoModuleSeqMax::runTask(): "
                "failed to acquire the input data lock");

        mainMutex.unlock();
        return; // data not up to date
    }

    if (attr.isStartSequence())
    {
        tmpMax = *pData;
        getInPorts()[inPortA]->releaseData();
    }
    else
    {
        if (*pData > tmpMax)
            tmpMax = *pData;

        if (!attr.isEndSequence())
        {
            getInPorts()[inPortA]->releaseData();
            mainMutex.unlock();
            return;
        }
        else
        {
            DataAttributeOut outAttr = attr;
            getInPorts()[inPortA]->releaseData();

            int* pOutData;

            // try to acquire the output data lock
            while (!getOutPorts()[outPortA]->tryData(pOutData))
            {
                poco_debug(logger(),
                        "DemoModuleSeqMax::runTask(): "
                        "failed to acquire the output data lock");

                if (sleep(TIME_LAPSE))
                {
                    mainMutex.unlock();
                    return;
                }
            }

            *pOutData = tmpMax;
            getOutPorts()[outPortA]->notifyReady(outAttr);

            poco_information(logger(), "DemoModuleSeqMax::runTask() outputs: "
                    + Poco::NumberFormatter::format(tmpMax));
        }
    }

    mainMutex.unlock();
}
