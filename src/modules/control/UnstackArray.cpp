/**
 * @file	src/modules/control/UnstackArray.cpp
 * @date	Jul. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#include "UnstackArray.h"

#include "Poco/NumberFormatter.h"

size_t UnstackArray::refCount = 0;

UnstackArray::UnstackArray(ModuleFactory* parent, std::string customName, int dataType):
	Module(parent, customName),
	mDataType(dataType)
{
    setInternalName("UnstackArray" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    setInPortCount(inPortCnt);
    addInPort("array", "Array to be unstacked", mDataType | DataItem::contVector, arrayInPort);

    setOutPortCount(outPortCnt);
    addOutPort("elements", "Output the array elements in a sequence", mDataType, dataOutPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

std::string UnstackArray::description()
{
	std::string descr = DataItem::dataTypeStr(mDataType);
	descr += " array unstacking Module. \n"
			"The lements of the input array are unstacked \n"
			"and sent as a sequence to the output. ";
	return descr;
}

void UnstackArray::process(int startCond)
{
    if (startCond == noDataStartState)
    {
        poco_information(logger(), name() + ": no input data. Exiting. ");
        return;
    }

    DataAttributeIn attr;
    readLockInPort(arrayInPort);
    readInPortDataAttribute(arrayInPort, &attr);

    switch (DataItem::noContainerDataType(mDataType))
    {
    case DataItem::typeInt32:
    {
    	std::vector<Poco::Int32>* pData;
    	readInPortData< std::vector<Poco::Int32> >(arrayInPort, pData);
    	sendData<Poco::Int32>(*pData, attr);
    	break;
    }
    case DataItem::typeUInt32:
    {
    	std::vector<Poco::UInt32>* pData;
    	readInPortData< std::vector<Poco::UInt32> >(arrayInPort, pData);
    	sendData<Poco::UInt32>(*pData, attr);
    	break;
    }
    case DataItem::typeInt64:
    {
    	std::vector<Poco::Int64>* pData;
    	readInPortData< std::vector<Poco::Int64> >(arrayInPort, pData);
    	sendData<Poco::Int64>(*pData, attr);
    	break;
    }
    case DataItem::typeUInt64:
    {
    	std::vector<Poco::UInt64>* pData;
    	readInPortData< std::vector<Poco::UInt64> >(arrayInPort, pData);
    	sendData<Poco::UInt64>(*pData, attr);
    	break;
    }

#ifdef HAVE_OPENCV
    case DataItem::typeCvMat:
    {
    	std::vector<cv::Mat>* pData;
    	readInPortData< std::vector<cv::Mat> >(arrayInPort, pData);
    	sendData<cv::Mat>(*pData, attr);
    	break;
    }
#endif
    case DataItem::typeFloat:
    {
    	std::vector<float>* pData;
    	readInPortData< std::vector<float> >(arrayInPort, pData);
    	sendData<float>(*pData, attr);
    	break;
    }
    case DataItem::typeDblFloat:
    {
    	std::vector<double>* pData;
    	readInPortData< std::vector<double> >(arrayInPort, pData);
    	sendData<double>(*pData, attr);
    	break;
    }
    case DataItem::typeString:
    {
    	std::vector<std::string>* pData;
    	readInPortData< std::vector<std::string> >(arrayInPort, pData);
    	sendData<std::string>(*pData, attr);
    	break;
    }
    default:
    	throw Poco::NotImplementedException("UnstackArray",
    			"data type not supported");
    }
}
