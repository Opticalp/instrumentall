/**
 * @file	src/DataGen.cpp
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

#include "OutPort.h"
#include "DataAttributeOut.h"

#include "TrigPort.h"

#include "Poco/NumberFormatter.h"

size_t DataGen::refCount = 0;

DataGen::DataGen(ModuleFactory* parent, std::string customName, int dataType):
	Module(parent, customName),
	mDataType(dataType),
	iPar(-1), fPar(-1),
	seqStart(false), seqEnd(false)
{
	setInternalName(DataItem::dataTypeShortStr(mDataType)
						+ "DataGen"
						+ Poco::NumberFormatter::format(refCount));
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
	descr += " data generator Module. "
			"The parameter sets the value that will be sent. "
			"The \"value\" parameter is stacked (fifo)! ";
	return descr;
}

void DataGen::run()
{
    dataLock.writeLock(); // write since the seq flags can be changed

    DataAttribute inAttr;

    bool trigged = false;

    if (reinterpret_cast<TrigPort*>(getInPorts()[trigPort])->tryDataAttribute(&inAttr))
    {
        poco_information(logger(),"trigged ------------------------------");
        trigged = true;
        (getInPorts()[trigPort])->release();

        if (attr.isSettingSequence())
        {
            dataLock.unlock();
            throw Poco::RuntimeException("DataGen",
                    "Concurrence between trig attribute "
                    "and module sequence parameters. ");
        }
    }

    if (!trigged)
    {
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
    }

    if (!trigged)
        attrQueue.push(attr++);
    else
        attrQueue.push(inAttr);

    // unstack and send just 1 element (fifo).
    poco_information(logger(),"queue size: "
            + Poco::NumberFormatter::format(attrQueue.size()));

    if (iQueue.size())
        poco_information(logger(),"iQueue front: "
            + Poco::NumberFormatter::format(iQueue.front()));

    dataLock.unlock();

    // try to acquire the output data lock
    runTaskMutex.lock();

	while (!tryData())
    {
        poco_information(logger(),
                "DatGen::runTask(): "
                "failed to acquire the output data lock. "
                "Wait " + Poco::NumberFormatter::format(TIME_LAPSE)
                + " ms now and retry. ");

        // the data lock is unlocked, data can be stacked on the queue
        // during sleep time,
        // but the main lock avoid another thread to acquire the output
        // port lock.

        if (sleep(TIME_LAPSE))
        {
            runTaskMutex.unlock();
            poco_notice(logger(), "DataGen::runTask(): cancelled!" );
            return;
        }
    }

    sendData();
    runTaskMutex.unlock();
}

bool DataGen::tryData()
{
	switch(mDataType)
	{
	case DataItem::typeInt32:
		return getOutPorts()[outPortData]->tryData<Poco::Int32>(pInt32);
	case DataItem::typeUInt32:
		return getOutPorts()[outPortData]->tryData<Poco::UInt32>(pUInt32);
	case DataItem::typeInt64:
		return getOutPorts()[outPortData]->tryData<Poco::Int64>(pInt64);
	case DataItem::typeUInt64:
		return getOutPorts()[outPortData]->tryData<Poco::UInt64>(pUInt64);
	case DataItem::typeFloat:
		return getOutPorts()[outPortData]->tryData<float>(pFloat);
	case DataItem::typeDblFloat:
		return getOutPorts()[outPortData]->tryData<double>(pDblFloat);
	case DataItem::typeString:
		return getOutPorts()[outPortData]->tryData<std::string>(pString);

    case DataItem::typeInt32 | DataItem::contVector:
        return getOutPorts()[outPortData]->tryData< std::vector<Poco::Int32> >(pVectInt32);
    case DataItem::typeUInt32 | DataItem::contVector:
        return getOutPorts()[outPortData]->tryData< std::vector<Poco::UInt32> >(pVectUInt32);
    case DataItem::typeInt64 | DataItem::contVector:
        return getOutPorts()[outPortData]->tryData< std::vector<Poco::Int64> >(pVectInt64);
    case DataItem::typeUInt64 | DataItem::contVector:
        return getOutPorts()[outPortData]->tryData< std::vector<Poco::UInt64> >(pVectUInt64);
    case DataItem::typeFloat | DataItem::contVector:
        return getOutPorts()[outPortData]->tryData< std::vector<float> >(pVectFloat);
    case DataItem::typeDblFloat | DataItem::contVector:
        return getOutPorts()[outPortData]->tryData< std::vector<double> >(pVectDblFloat);
    case DataItem::typeString | DataItem::contVector:
        return getOutPorts()[outPortData]->tryData< std::vector<std::string> >(pVectString);

    default:
		// already verified in constructor!
    	poco_bugcheck_msg("DataGen::tryData >> data type not supported");
    	throw Poco::BugcheckException();
	}
}

void DataGen::sendData()
{
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

    default:
        // already verified in constructor!
        poco_bugcheck_msg("DataGen::sendData >> data type not supported");
        throw Poco::BugcheckException();
    }

    DataAttributeOut attrOut(attrQueue.front());
    attrQueue.pop();

    // done with the module data
	dataLock.unlock();

	getOutPorts()[outPortData]->notifyReady(attrOut);

	poco_information(logger(),"DataGen: data sent");
}

long DataGen::getIntParameterValue(size_t paramIndex)
{
    long ret;
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

void DataGen::setIntParameterValue(size_t paramIndex, long value)
{
	dataLock.writeLock();

	switch (paramIndex)
	{
	case paramValue:
	    iQueue.push(value);
	    iPar = value;
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

