/**
 * @file	src/DemoModuleTwoInputs.cpp
 * @date	june 2016
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

#include "DemoModuleTwoInputs.h"
#include "Poco/NumberFormatter.h"

size_t DemoModuleTwoInputs::refCount = 0;

DemoModuleTwoInputs::DemoModuleTwoInputs(ModuleFactory* parent, std::string customName):
        Module(parent, customName)
{
    setInternalName("DemoModuleTwoInputs" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // ports
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addInPort("portA", "data in", DataItem::typeDblFloat, inPortA);
    addInPort("portB", "data in", DataItem::typeDblFloat, inPortB);

    addOutPort("portA", "forwarded data as is", DataItem::typeDblFloat, outPortA);

    notifyCreation();

    // if nothing failed
    refCount++;
}

void DemoModuleTwoInputs::process(int startCond)
{
	// in case of a motion stage for example,
    // to be sure that the targets are done with
	// what had to be done at the previous position.
    reserveAllOutPorts();

    DataAttributeIn attrs[inPortCnt];

    switch (startCond)
    {
    case allDataStartState:
		readLockInPort(inPortA);
    	readInPortDataAttribute(inPortA, attrs + inPortA);
		readLockInPort(inPortB);
    	readInPortDataAttribute(inPortB, attrs + inPortB);
    	Poco::Thread::sleep(1000); // simulate a job (ms)
    	releaseAllInPorts(); // release when the job is done
    	break;
    case noDataStartState:
    	attrs[inPortB] = attrs[inPortA];
    	break;
    default:
    	poco_bugcheck_msg("impossible start condition");
    	throw Poco::BugcheckException();
    }

    double* outData[outPortCnt];
    getDataToWrite<double>(outPortA, outData[outPortA]);

    *outData[outPortA] = 100;

    notifyOutPortReady(outPortA, attrs[inPortA]);
}
