/**
 * @file	src/DemoModuleDataSeq.cpp
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

#include "DemoModuleDataSeq.h"

#include "core/OutPort.h"
#include "core/DataAttributeOut.h"

#include "Poco/NumberFormatter.h"

size_t DemoModuleDataSeq::refCount = 0;

DemoModuleDataSeq::DemoModuleDataSeq(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    // poco_information(logger(),"Creating a new DemoModuleDataSeq");

    setInternalName("DemoModuleDataSeq" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // ports
    setOutPortCount(outPortCnt);
    addOutPort("outPortA", "generated data sequence",
            DataItem::typeInteger, outPortA);

    notifyCreation();

    // if nothing failed
    refCount++;
}

#define MAX_INDEX 8
#define WAIT_TIME 200

void DemoModuleDataSeq::process(int startCond)
{
    DataAttributeOut attr;

    attr.startSequence();

    for (int index = 0; index < MAX_INDEX + 1; index++)
    {
        int *pData;

        // lock outPortA
        poco_information(logger(), name() + ": trying to reserve outPort");
        reserveOutPort(outPortA);
        poco_information(logger(), name() + ": outPort reserved");
        // retrieve data access
        getDataToWrite(outPortA, pData);

        if (index==MAX_INDEX)
            attr.endSequence(); // set end sequence to the attribute

        *pData = index;
        notifyOutPortReady(outPortA, attr++);

        poco_information(logger(), "DemoModuleDataSeq::runTask(): sent "
                + Poco::NumberFormatter::format(index));

        setProgress(static_cast<float>(index + 1) / static_cast<float>(MAX_INDEX + 1));

        if (sleep(WAIT_TIME))
            throw ExecutionAbortedException(name(), "Cancelled upon user request" );
    }

    poco_information(logger(), "DemoModuleDataSeq::runTask(): all sent. ");
}
