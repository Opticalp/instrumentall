/**
 * detailed comment
 * 
 * @file	/Instrumentall-Debug@instru-git-debug/[Source directory]/src/DataGen.cpp
 * @brief	short comment
 * @date	26 mars 2016
 * @author	PhRG - opticalp.fr
 *
 * $Id$
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

#include "Poco/NumberFormatter.h"

size_t DataGen::refCount = 0;

DataGen::DataGen(ModuleFactory* parent, std::string customName, int dataType):
	Module(parent),
	mDataType(dataType)
{
	setInternalName(DataItem::dataTypeShortStr(mDataType)
						+ "DataGen"
						+ Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

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
	dataLock.readLock();

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

    DataAttributeOut attr = DataAttributeOut::newDataAttribute();

    setData();
    getOutPorts()[outPortData]->notifyReady(attr);

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
	poco_assert(paramIndex == paramValue);
	dataLock.readLock();
	long tmp = iPar;
	dataLock.unlock();
	return tmp;
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
	poco_assert(paramIndex == paramValue);
	dataLock.writeLock();
	iPar = value;
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

