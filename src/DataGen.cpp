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
	iPar(0), fPar(0),
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

    switch (mDataType)
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
    		"value that will be exported on the data output port",
    		paramType);

    addParameter(paramSeqStart,
    		"seqStart",
    		"define if the data to be sent is a start sequence",
    		ParamItem::typeInteger);
    addParameter(paramSeqEnd,
    		"seqEnd",
    		"define if the data to be sent is a end sequence",
    		ParamItem::typeInteger);

    notifyCreation();

    // if nothing failed
    refCount++;
}

std::string DataGen::description()
{
	std::string descr = DataItem::dataTypeStr(mDataType);
	descr += " data generator Module. "
			"The parameter sets the value that will be sent. ";
	return descr;
}

void DataGen::runTask()
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
            throw Poco::RuntimeException("DatGen",
                    "Concurrence between trig attribute "
                    "and modulesequence parameters. ");
    }

    // try to acquire the output data lock
    while (!tryData())
    {
        poco_information(logger(),
                "DatGen::runTask(): "
                "failed to acquire the output data lock. "
                "Wait " + Poco::NumberFormatter::format(TIME_LAPSE)
                + " ms now and retry. ");

        if (sleep(TIME_LAPSE))
        {
            poco_notice(logger(), "DataGen::runTask(): cancelled!" );
        	dataLock.unlock();
            return;
        }
    }

    if (!trigged)
    {
        for (; seqStart > 0; seqStart--)
            attr.startSequence();

        // FIXME: should check that they are not sent more end sequence
        // than start sequence.
        for (; seqEnd > 0; seqEnd--)
            attr.endSequence();
    }

    setData();

    if (!trigged)
        getOutPorts()[outPortData]->notifyReady(attr++);
    else
        getOutPorts()[outPortData]->notifyReady(DataAttributeOut(inAttr));

	poco_information(logger(), "DataGen : data sent");

	dataLock.unlock();
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
	default:
		// already verified in constructor!
    	poco_bugcheck_msg("DataGen::tryData >> data type not supported");
    	throw Poco::BugcheckException();
	}
}

void DataGen::setData()
{
	switch(mDataType)
	{
	case DataItem::typeInt32:
		*pInt32 = static_cast<Poco::Int32>(iPar);
		break;
	case DataItem::typeUInt32:
		*pUInt32 = static_cast<Poco::UInt32>(iPar);
		break;
	case DataItem::typeInt64:
		*pInt64 = static_cast<Poco::Int64>(iPar);
		break;
	case DataItem::typeUInt64:
		*pUInt64 = static_cast<Poco::UInt64>(iPar);
		break;
	case DataItem::typeFloat:
		*pFloat = static_cast<float>(fPar);
		break;
	case DataItem::typeDblFloat:
		*pDblFloat = fPar;
		break;
	case DataItem::typeString:
		*pString = sPar;
		break;
	default:
		// already verified in constructor!
    	poco_bugcheck_msg("DataGen::setData >> data type not supported");
    	throw Poco::BugcheckException();
	}
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
	fPar = value;
	dataLock.unlock();
}

void DataGen::setStrParameterValue(size_t paramIndex, std::string value)
{
	poco_assert(paramIndex == paramValue);
	dataLock.writeLock();
	sPar = value;
	dataLock.unlock();
}

