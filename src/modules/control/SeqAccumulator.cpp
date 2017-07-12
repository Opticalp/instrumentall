/**
 * @file	src/modules/control/SeqAccumulator.cpp
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

#include "SeqAccumulator.h"

#include "Poco/NumberFormatter.h"

size_t SeqAccumulator::refCount = 0;

SeqAccumulator::SeqAccumulator(ModuleFactory* parent, std::string customName, int dataType):
	Module(parent, customName),
	mDataType(dataType),
	dataStore(dataType | DataItem::contVector),
	seqIndex(0)
{
    setInternalName("SeqAccumulator" + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    setInPortCount(inPortCnt);
    addInPort("elements", "seq data to be accumulated", mDataType, dataInPort);

    setOutPortCount(outPortCnt);
    addOutPort("array", "Output the array of elements of one sequence", mDataType | DataItem::contVector, arrayOutPort);

    notifyCreation();

    // if nothing failed
    refCount++;
}

std::string SeqAccumulator::description()
{
	std::string descr = DataItem::dataTypeStr(mDataType);
	descr += " data sequence accumulator Module. \n"
			"The elements of the input sequence are stacked \n"
			"and sent as an array to the output. ";
	return descr;
}

void SeqAccumulator::process(int startCond)
{
    if (startCond == noDataStartState)
    {
        poco_information(logger(), name() + ": no input data. Exiting. ");
        return;
    }

    DataAttributeIn attr;
    readLockInPort(dataInPort);
    readInPortDataAttribute(dataInPort, &attr);

    bool end = false;

    if (attr.isStartSequence(seqIndex))
    {
        poco_information(logger(), name() + ": sequence starting");
        clearStore();
    }

    if (attr.isInSequence(seqIndex))
    {
        if (attr.isEndSequence(seqIndex))
        {
        	poco_information(logger(), name() + ": sequence ending");
        	end = true;
        }
//        else
//        {
//            keepParamLocked(); // not requested here since there is no parameter
//        }
    }
    else
        end = true; // send a 1-element array

    appendDataToStore();

    releaseInPort(dataInPort);

    if (end)
    {
        DataAttributeOut outAttr = attr;

        reserveOutPort(arrayOutPort);
        writeOutData();

        notifyOutPortReady(arrayOutPort, outAttr);
    }
}

void SeqAccumulator::appendDataToStore()
{
    switch (DataItem::noContainerDataType(mDataType))
    {
    case DataItem::typeInt32:
    {
    	Poco::Int32* pData;
    	readInPortData<Poco::Int32>(dataInPort, pData);
    	dataStore.getData< std::vector<Poco::Int32> >()->push_back(*pData);
    	break;
    }
    case DataItem::typeUInt32:
    {
    	Poco::UInt32* pData;
    	readInPortData<Poco::UInt32>(dataInPort, pData);
    	dataStore.getData< std::vector<Poco::UInt32> >()->push_back(*pData);
    	break;
    }
    case DataItem::typeInt64:
    {
    	Poco::Int64* pData;
    	readInPortData<Poco::Int64>(dataInPort, pData);
    	dataStore.getData< std::vector<Poco::Int64> >()->push_back(*pData);
    	break;
    }
    case DataItem::typeUInt64:
    {
    	Poco::UInt64* pData;
    	readInPortData<Poco::UInt64>(dataInPort, pData);
    	dataStore.getData< std::vector<Poco::UInt64> >()->push_back(*pData);
    	break;
    }

#ifdef HAVE_OPENCV
    case DataItem::typeCvMat:
    {
    	cv::Mat* pData;
    	readInPortData<cv::Mat>(dataInPort, pData);
    	dataStore.getData< std::vector<cv::Mat> >()->push_back(*pData);
    	break;
    }
#endif
    case DataItem::typeFloat:
    {
    	float* pData;
    	readInPortData<float>(dataInPort, pData);
    	dataStore.getData< std::vector<float> >()->push_back(*pData);
    	break;
    }
    case DataItem::typeDblFloat:
    {
    	double* pData;
    	readInPortData<double>(dataInPort, pData);
    	dataStore.getData< std::vector<double> >()->push_back(*pData);
    	break;
    }
    case DataItem::typeString:
    {
    	std::string* pData;
    	readInPortData<std::string>(dataInPort, pData);
    	dataStore.getData< std::vector<std::string> >()->push_back(*pData);
    	break;
    }
    default:
    	throw Poco::NotImplementedException("SeqAccumulator",
    			"data type not supported");
    }
}

void SeqAccumulator::clearStore()
{
    switch (DataItem::noContainerDataType(mDataType))
    {
    case DataItem::typeInt32:
    	dataStore.getData< std::vector<Poco::Int32> >()->clear();
    	break;
    case DataItem::typeUInt32:
    	dataStore.getData< std::vector<Poco::UInt32> >()->clear();
    	break;
    case DataItem::typeInt64:
    	dataStore.getData< std::vector<Poco::Int64> >()->clear();
    	break;
    case DataItem::typeUInt64:
    	dataStore.getData< std::vector<Poco::UInt64> >()->clear();
    	break;

#ifdef HAVE_OPENCV
    case DataItem::typeCvMat:
    	dataStore.getData< std::vector<cv::Mat> >()->clear();
    	break;
#endif
    case DataItem::typeFloat:
    	dataStore.getData< std::vector<float> >()->clear();
    	break;
    case DataItem::typeDblFloat:
    	dataStore.getData< std::vector<double> >()->clear();
    	break;
    case DataItem::typeString:
    	dataStore.getData< std::vector<std::string> >()->clear();
    	break;
    default:
    	throw Poco::NotImplementedException("SeqAccumulator",
    			"data type not supported");
    }
}

void SeqAccumulator::writeOutData()
{
    switch (DataItem::noContainerDataType(mDataType))
    {
    case DataItem::typeInt32:
    {
    	std::vector<Poco::Int32>* pData;
    	getDataToWrite< std::vector<Poco::Int32> >(arrayOutPort, pData);
    	*pData = std::vector<Poco::Int32>(*dataStore.getData< std::vector<Poco::Int32> >());
    	break;
    }
    case DataItem::typeUInt32:
    {
    	std::vector<Poco::UInt32>* pData;
    	getDataToWrite< std::vector<Poco::UInt32> >(arrayOutPort, pData);
    	*pData = std::vector<Poco::UInt32>(*dataStore.getData< std::vector<Poco::UInt32> >());
    	break;
    }
    case DataItem::typeInt64:
    {
    	std::vector<Poco::Int64>* pData;
    	getDataToWrite< std::vector<Poco::Int64> >(arrayOutPort, pData);
    	*pData = std::vector<Poco::Int64>(*dataStore.getData< std::vector<Poco::Int64> >());
    	break;
    }
    case DataItem::typeUInt64:
    {
    	std::vector<Poco::UInt64>* pData;
    	getDataToWrite< std::vector<Poco::UInt64> >(arrayOutPort, pData);
    	*pData = std::vector<Poco::UInt64>(*dataStore.getData< std::vector<Poco::UInt64> >());
    	break;
    }

#ifdef HAVE_OPENCV
    case DataItem::typeCvMat:
    {
    	std::vector<cv::Mat>* pData;
    	getDataToWrite< std::vector<cv::Mat> >(arrayOutPort, pData);
    	*pData = std::vector<cv::Mat>(*dataStore.getData< std::vector<cv::Mat> >());
    	break;
    }
#endif
    case DataItem::typeFloat:
    {
    	std::vector<float>* pData;
    	getDataToWrite< std::vector<float> >(arrayOutPort, pData);
    	*pData = std::vector<float>(*dataStore.getData< std::vector<float> >());
    	break;
    }
    case DataItem::typeDblFloat:
    {
    	std::vector<double>* pData;
    	getDataToWrite< std::vector<double> >(arrayOutPort, pData);
    	*pData = std::vector<double>(*dataStore.getData< std::vector<double> >());
    	break;
    }
    case DataItem::typeString:
    {
    	std::vector<std::string>* pData;
    	getDataToWrite< std::vector<std::string> >(arrayOutPort, pData);
    	*pData = std::vector<std::string>(*dataStore.getData< std::vector<std::string> >());
    	break;
    }
    default:
    	throw Poco::NotImplementedException("SeqAccumulator",
    			"data type not supported");
    }
}

void SeqAccumulator::reset()
{
	clearStore();
	seqIndex = 0;
}
