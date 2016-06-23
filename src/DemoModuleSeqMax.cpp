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

#include "InDataPort.h"
#include "OutPort.h"

#include "Poco/NumberFormatter.h"

size_t DemoModuleSeqMax::refCount = 0;

DemoModuleSeqMax::DemoModuleSeqMax(ModuleFactory* parent, std::string customName):
    Module(parent, customName),
    seqIndex(0),
    tmpMax(0) // no special meaning for this value
{
    // poco_information(logger(),"Creating a new DemoModuleSeqMax");

    setInternalName("DemoModuleSeqMax" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("inPortA", "data sequence", DataItem::typeInteger, inPortA);

    addOutPort("outPortA", "max of data sequence", DataItem::typeInteger, outPortA);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void DemoModuleSeqMax::process(int startCond)
{
	if (startCond != allDataStartState)
	{
		poco_information(logger(), name() + ": no input data. Exiting. ");
		return;
	}

    DataAttributeIn attr;
    int* pData;
    readInPortData<int>(inPortA, pData);
    readInPortDataAttribute(inPortA, &attr);

    if (attr.isStartSequence(seqIndex))
    {
        tmpMax = *pData;
    }
    else if (attr.isInSequence(seqIndex))
    {
        if (*pData > tmpMax)
            tmpMax = *pData;

//        // emulate a long task
//        if (sleep(10*TIME_LAPSE))
//        {
//            poco_notice(logger(),
//                    "DemoModuleSeqMax::runTask(): cancelled!");
//            return;
//        }
//
        if (attr.isEndSequence(seqIndex))
        {
            DataAttributeOut outAttr = attr;
            releaseInPort(inPortA);

            reserveOutPort(outPortA);

            int* pOutData;
            getDataToWrite<int>(outPortA, pOutData);

            *pOutData = tmpMax;
            notifyOutPortReady(outPortA, outAttr);

            poco_information(logger(), "DemoModuleSeqMax::runTask() outputs: "
                    + Poco::NumberFormatter::format(tmpMax));
        }
    }
    else
        throw Poco::RuntimeException("DemoModuleSeqMax::process",
                "not able to process data out of a sequence...");
}

void DemoModuleSeqMax::reset()
{
	seqIndex = 0;
}
