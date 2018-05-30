/**
 * @file	src/modules/dataGen/DataGen.cpp
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

#include "DataGen.h"

#include "core/OutPort.h"
#include "core/DataAttributeOut.h"

#include "core/TrigPort.h"

#include "Poco/NumberFormatter.h"

size_t DataGen::refCount = 0;

DataGen::DataGen(ModuleFactory* parent, std::string customName, int dataType):
	Module(parent, customName),
	mDataType(dataType),
	iPar(-1), fPar(-1),
	seqStart(false), seqEnd(false)
{
    if (refCount)
        setInternalName(DataItem::dataTypeShortStr(mDataType)
						+ "DataGen"
						+ Poco::NumberFormatter::format(refCount));
    else
        setInternalName(DataItem::dataTypeShortStr(mDataType)
                        + "DataGen");

    setCustomName(customName);
    setLogger("module." + name());

    setInPortCount(inPortCnt);
    addTrigPort("trig", "Launch the data generation", trigPort);

    setOutPortCount(outPortCnt);
    addOutPort("data", "Output the data defined in the parameter", mDataType, outPortData);

    // parameters
    setParameterCount(paramCnt);

    ParamItem::ParamType paramType;

    switch (DataItem::noContainerDataType(mDataType))
    {
    case DataItem::typeInt32:
    case DataItem::typeUInt32:
    case DataItem::typeInt64:
    case DataItem::typeUInt64:
#ifdef HAVE_OPENCV
    case DataItem::typeCvMat:
#endif
    	paramType = ParamItem::typeInteger;
    	break;
    case DataItem::typeFloat:
    case DataItem::typeDblFloat:
    	paramType = ParamItem::typeFloat;
    	break;
    case DataItem::typeString:
    	paramType = ParamItem::typeString;
    	break;
    default:
    	throw Poco::NotImplementedException("DataGen::DataGen",
    			"data type not supported");
    }

    addParameter(paramValue,
    		"value",
    		"value that will be exported on the data output port. "
    		"This parameter is stacked (fifo). ",
    		paramType);

    addParameter(paramSeqStart,
    		"seqStart",
    		"define if the data to be sent is a start sequence",
    		ParamItem::typeInteger);
    addParameter(paramSeqEnd,
    		"seqEnd",
    		"define if the data to be sent is a end sequence. "
    		"You should wait for the end of the execution of this module, "
    		"before setting it, or it could be applied to the wrong value. ",
    		ParamItem::typeInteger);

    notifyCreation();

    // if nothing failed
    refCount++;
}

std::string DataGen::description()
{
	std::string descr = DataItem::dataTypeStr(mDataType);
	descr += " data generator Module. \n"
			"The parameter sets the value that will be sent. \n"
			"The \"value\" parameter is stacked (fifo)! ";
	return descr;
}

void DataGen::process(int startCond)
{
    dataLock.writeLock(); // write since the seq flags can be changed

    switch (startCond)
    {
    case noDataStartState:
    	freeRun();
    	break;
    case allDataStartState:
    	triggedRun();
    	break;
    default:
    	poco_bugcheck_msg("impossible start condition");
    	throw Poco::BugcheckException();
    }

    // unstack and send just 1 element (fifo).
    poco_information(logger(),"queue size: "
            + Poco::NumberFormatter::format(attrQueue.size()));

    if (iQueue.size())
        poco_information(logger(),"iQueue front: "
            + Poco::NumberFormatter::format(iQueue.front()));
    else
    	poco_information(logger(),"not working with iQueue");

    dataLock.unlock();

    // launch next task if requested.
    processingTerminated();

    reserveOutPort(outPortData);

    poco_information(logger(),"out port reserved");

    sendData();
}

void DataGen::freeRun()
{
	poco_information(logger(),"DataGen: free run");

    for (; seqStart > 0; seqStart--)
    {
        poco_information(logger(),name()+":startSeq");
        attr.startSequence();
    }

    // FIXME: should check that they are not sent more end sequence
    // than start sequence.
    for (; seqEnd > 0; seqEnd--)
    {
        poco_information(logger(),name()+":endSeq");
        attr.endSequence();
    }

    attrQueue.push(attr++);
}

void DataGen::triggedRun()
{
	poco_information(logger(),"DataGen: trigged run");

    DataAttributeIn inAttr;

	readLockInPort(trigPort);
    readInPortDataAttribute(trigPort, &inAttr);
    releaseInPort(trigPort);

    if (attr.isSettingSequence())
    {
        dataLock.unlock();
        throw Poco::RuntimeException("DataGen",
                "Concurrence between trig attribute "
                "and module sequence parameters. ");
    }

    attrQueue.push(inAttr);
}

void DataGen::sendData()
{
	switch(mDataType)
	{
	case DataItem::typeInt32:
		getDataToWrite<Poco::Int32>(outPortData, pInt32);
		break;
	case DataItem::typeUInt32:
		getDataToWrite<Poco::UInt32>(outPortData, pUInt32);
		break;
	case DataItem::typeInt64:
		getDataToWrite<Poco::Int64>(outPortData, pInt64);
		break;
	case DataItem::typeUInt64:
		getDataToWrite<Poco::UInt64>(outPortData, pUInt64);
		break;
	case DataItem::typeFloat:
		getDataToWrite<float>(outPortData, pFloat);
		break;
	case DataItem::typeDblFloat:
		getDataToWrite<double>(outPortData, pDblFloat);
		break;
	case DataItem::typeString:
		getDataToWrite<std::string>(outPortData, pString);
		break;
#ifdef HAVE_OPENCV
    case DataItem::typeCvMat:
        getDataToWrite<cv::Mat>(outPortData, pCvMat);
        break;
#endif

    case DataItem::typeInt32 | DataItem::contVector:
        getDataToWrite< std::vector<Poco::Int32> >(outPortData, pVectInt32);
		break;
    case DataItem::typeUInt32 | DataItem::contVector:
        getDataToWrite< std::vector<Poco::UInt32> >(outPortData, pVectUInt32);
		break;
    case DataItem::typeInt64 | DataItem::contVector:
        getDataToWrite< std::vector<Poco::Int64> >(outPortData, pVectInt64);
		break;
    case DataItem::typeUInt64 | DataItem::contVector:
        getDataToWrite< std::vector<Poco::UInt64> >(outPortData, pVectUInt64);
		break;
    case DataItem::typeFloat | DataItem::contVector:
        getDataToWrite< std::vector<float> >(outPortData, pVectFloat);
		break;
    case DataItem::typeDblFloat | DataItem::contVector:
        getDataToWrite< std::vector<double> >(outPortData, pVectDblFloat);
		break;
    case DataItem::typeString | DataItem::contVector:
        getDataToWrite< std::vector<std::string> >(outPortData, pVectString);
		break;
#ifdef HAVE_OPENCV
    case DataItem::typeCvMat | DataItem::contVector:
        getDataToWrite< std::vector<cv::Mat> >(outPortData, pVectCvMat);
        break;
#endif

    default:
		// already verified in constructor!
    	poco_bugcheck_msg("DataGen::tryData >> data type not supported");
    	throw Poco::BugcheckException();
	}

	dataLock.writeLock();

    poco_information(logger(),"sendData, queue size: "
            + Poco::NumberFormatter::format(attrQueue.size()));

    if (attrQueue.size() == 0)
        poco_bugcheck_msg("data queue is empty?!");

    if (iQueue.size() == 0)
        iQueue.push(iPar);
    if (fQueue.size() == 0)
        fQueue.push(fPar);
    if (sQueue.size() == 0)
        sQueue.push(sPar);

    switch(mDataType)
    {
    case DataItem::typeInt32:
        *pInt32 = static_cast<Poco::Int32>(iQueue.front());
        iQueue.pop();
        break;
    case DataItem::typeUInt32:
        *pUInt32 = static_cast<Poco::UInt32>(iQueue.front());
        iQueue.pop();
        break;
    case DataItem::typeInt64:
        *pInt64 = static_cast<Poco::Int64>(iQueue.front());
        iQueue.pop();
        break;
    case DataItem::typeUInt64:
        *pUInt64 = static_cast<Poco::UInt64>(iQueue.front());
        iQueue.pop();
        break;
    case DataItem::typeFloat:
        *pFloat = static_cast<float>(fQueue.front());
        fQueue.pop();
        break;
    case DataItem::typeDblFloat:
        *pDblFloat = fQueue.front();
        fQueue.pop();
        break;
    case DataItem::typeString:
        *pString = sQueue.front();
        sQueue.pop();
        break;
#ifdef HAVE_OPENCV
    case DataItem::typeCvMat:
        *pCvMat = fillOutCvMat(iQueue.front());
        iQueue.pop();
        break;
#endif

    case DataItem::typeInt32 | DataItem::contVector:
        *pVectInt32 = fillOutIntVect<Poco::Int32>(DataItem::typeInt32);
        break;
    case DataItem::typeUInt32 | DataItem::contVector:
        *pVectUInt32 = fillOutIntVect<Poco::UInt32>(DataItem::typeUInt32);
        break;
    case DataItem::typeInt64 | DataItem::contVector:
        *pVectInt64 = fillOutIntVect<Poco::Int64>(DataItem::typeInt64);
        break;
    case DataItem::typeUInt64 | DataItem::contVector:
        *pVectUInt64 = fillOutIntVect<Poco::UInt64>(DataItem::typeUInt64);
        break;
    case DataItem::typeFloat | DataItem::contVector:
        *pVectFloat = fillOutFloatVect<float>(DataItem::typeFloat);
        break;
    case DataItem::typeDblFloat | DataItem::contVector:
        *pVectDblFloat = fillOutFloatVect<double>(DataItem::typeDblFloat);
        break;
    case DataItem::typeString | DataItem::contVector:
        *pVectString = fillOutStrVect();
        break;
#ifdef HAVE_OPENCV
    case DataItem::typeCvMat | DataItem::contVector:
        *pVectCvMat = fillOutCvMatVect();
        break;
#endif

    default:
        // already verified in constructor!
        poco_bugcheck_msg("DataGen::sendData >> data type not supported");
        throw Poco::BugcheckException();
    }

    DataAttributeOut attrOut(attrQueue.front());
    attrQueue.pop();

    // done with the module data
	dataLock.unlock();

	notifyOutPortReady(outPortData, attrOut);

	poco_information(logger(),"DataGen: data sent");
}

Poco::Int64 DataGen::getIntParameterValue(size_t paramIndex)
{
    Poco::Int64 ret;
	dataLock.readLock();
	switch (paramIndex)
	{
	case paramValue:
		ret = iPar;
		break;
	case paramSeqStart:
		ret = seqStart;
		break;
	case paramSeqEnd:
		ret = seqEnd;
		break;
	default:
	    dataLock.unlock();
		poco_bugcheck_msg("getIntParameterValue: wrong index");
		throw Poco::BugcheckException();
	}

	dataLock.unlock();
	return ret;
}


double DataGen::getFloatParameterValue(size_t paramIndex)
{
	poco_assert(paramIndex == paramValue);
	dataLock.readLock();
	double tmp = fPar;
	dataLock.unlock();
	return tmp;
}

std::string DataGen::getStrParameterValue(size_t paramIndex)
{
	poco_assert(paramIndex == paramValue);
	dataLock.readLock();
	std::string tmp = sPar;
	dataLock.unlock();
	return tmp;
}

void DataGen::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
	dataLock.writeLock();

	switch (paramIndex)
	{
	case paramValue:
	    iQueue.push(value);
	    iPar = value;
	    poco_information(logger(), name() + ": int param set");
		break;
	case paramSeqStart:
		seqStart = value;
		break;
	case paramSeqEnd:
        seqEnd = value;
		break;
	default:
	    dataLock.unlock();
		poco_bugcheck_msg("setIntParameterValue: wrong index");
		throw Poco::BugcheckException();
	}

	dataLock.unlock();
}

void DataGen::setFloatParameterValue(size_t paramIndex, double value)
{
	poco_assert(paramIndex == paramValue);
	dataLock.writeLock();
    fQueue.push(value);
    fPar = value;
	dataLock.unlock();
}

void DataGen::setStrParameterValue(size_t paramIndex, std::string value)
{
	poco_assert(paramIndex == paramValue);
	dataLock.writeLock();
    sQueue.push(value);
    sPar = value;
	dataLock.unlock();
}

void DataGen::reset()
{
	Poco::ScopedWriteRWLock lock(dataLock);

	attr = DataAttributeOut();

    std::queue<Poco::Int64> iEmpty;
    std::swap(iEmpty, iQueue);
    std::queue<double> fEmpty;
    std::swap(fEmpty, fQueue);
    std::queue<std::string> sEmpty;
    std::swap(sEmpty, sQueue);
    std::queue<DataAttributeOut> attrEmpty;
    std::swap(attrEmpty, attrQueue);

    seqStart = 0;
    seqEnd = 0;
}
