/**
 * @file	src/DemoModuleSeqAccu.cpp
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

#include "DemoModuleSeqAccu.h"

#include "core/DataAttributeIn.h"
#include "core/DataAttribute.h"

#include "core/InDataPort.h"
#include "core/OutPort.h"

#include "Poco/NumberFormatter.h"

size_t DemoModuleSeqAccu::refCount = 0;

DemoModuleSeqAccu::DemoModuleSeqAccu(ModuleFactory* parent, std::string customName):
    Module(parent, customName),
    seqIndex(0),
    accumulator(0)
{
    // poco_information(logger(),"Creating a new DemoModuleSeqAccu");

    setInternalName("DemoModuleSeqAccu" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("inPortA", "data sequence", DataItem::typeInteger, inPortA);

    addOutPort("outPortA", "data sequence as a vector",
            DataItem::typeInteger | DataItem::contVector, outPortA);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void DemoModuleSeqAccu::process(int startCond)
{
	if (startCond != allDataStartState)
	{
		poco_information(logger(), name() + ": no input data. Exiting. ");
		return;
	}

	DataAttributeIn attr;
    Poco::Int32* pData;

	readLockInPort(inPortA);
    readInPortData<Poco::Int32>(inPortA, pData);
    readInPortDataAttribute(inPortA, &attr);

    if (attr.isStartSequence(seqIndex))
    {
        poco_information(logger(), "DemoModuleSeqAccu::runTask(): sequence starting with "
                + Poco::NumberFormatter::format(*pData));
        accumulator.clear();
        accumulator.push_back(*pData);

//        // emulate a long task
//        if (sleep(10*TIME_LAPSE))
//        {
//            poco_notice(logger(),
//                    "DemoModuleSeqMax::runTask(): cancelled!");
//            return;
//        }
    }
    else if (attr.isInSequence(seqIndex))
    {
        accumulator.push_back(*pData);

//        // emulate a long task
//        if (sleep(10*TIME_LAPSE))
//        {
//            poco_notice(logger(),
//                    "DemoModuleSeqMax::runTask(): cancelled!");
//            return;
//        }

        if (attr.isEndSequence(seqIndex))
        {
            poco_information(logger(), "DemoModuleSeqAccu::runTask(): sequence ending");
            DataAttributeOut outAttr = attr;
            releaseInPort(inPortA);

            std::vector<Poco::Int32>* pOutData;

            reserveOutPort(outPortA);
            getDataToWrite< std::vector<Poco::Int32> >(outPortA, pOutData);

            *pOutData = accumulator;
            notifyOutPortReady(outPortA, outAttr);
        }
    }
    else
        throw Poco::RuntimeException("DemoModuleSeqAccu::process",
                "not able to process data out of a sequence...");
}

void DemoModuleSeqAccu::reset()
{
	seqIndex = 0;
	accumulator.clear();
}

void DemoModuleSeqAccu::cancel()
{
	poco_warning(logger(), name() + " cancelling...");
}
